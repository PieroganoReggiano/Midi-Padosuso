#include "Statek.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <exception>
#include <cstring>
#include <string>
#include <linux/uinput.h>
#include <csignal>

using namespace std::string_literals;

volatile static bool *p_not_end;

namespace padosuso {

Statek::Statek() {
  m_fd = -1;
  m_p_map = nullptr;
  init_midi();
}

Statek::~Statek() {
  deactivate_map_and_uinput();
  // Midi doesn't need to deactivate beacue of destructor.
}

void Statek::activate_map(const Map* p_map) {

  if (pad_activated()) {
    deactivate_map_and_uinput();
  }

  if (m_fd > -1) close(m_fd);

  m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (m_fd < 0) {
    throw std::runtime_error("could not open /dev/uinput");
  }

  std::memset(&m_usetup, 0, sizeof(m_usetup));
  m_usetup.id.bustype = BUS_USB;
  m_usetup.id.vendor = 0xffff;
  m_usetup.id.product = 0x0000;
  m_usetup.id.version = 0xffff;
  strcpy(m_usetup.name, "Padosuso");

  ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
  for (auto it = p_map->classic_keys.cbegin();
      it != p_map->classic_keys.cend();
      ++it) {
    ClassicKey ck(*it);
    ioctl(m_fd, UI_SET_KEYBIT, ck.key);
  }

  if (p_map->two_way_keys.size() > 0) {
    ioctl(m_fd, UI_SET_EVBIT, EV_ABS);
    //ioctl(m_fd, UI_SET_EVBIT, EV_FF);
  }
  for (auto it = p_map->two_way_keys.cbegin();
      it != p_map->two_way_keys.cend();
      ++it) {
    TwoWayKey twk(*it);
    ioctl(m_fd, UI_SET_ABSBIT, twk.key);
    struct uinput_abs_setup abs_setup;
    std::memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = twk.key;
    abs_setup.absinfo.minimum = -1;
    abs_setup.absinfo.maximum = 1;
    ioctl(m_fd, UI_ABS_SETUP, &abs_setup);
  }


  ioctl(m_fd, UI_DEV_SETUP, &m_usetup);
  ioctl(m_fd, UI_DEV_CREATE);

  m_key_states.clear();

  for (auto it = p_map->two_way_keys.cbegin();
      it != p_map->two_way_keys.cbegin();
      ++it) {
    m_key_states[it->key] = 0;
  }

  m_p_map = p_map;
  usleep(100);
}

void Statek::deactivate_map_and_uinput() {
  if (m_fd >= 0) {
    ioctl(m_fd, UI_DEV_DESTROY);
    close(m_fd);
  }
}

void Statek::use_midi(unsigned number) {
  if (!midi_activated()) {
    throw std::logic_error("cannot use midi input while RtMidi is not "
      "initialized");
  }
  m_up_midiin->openPort(number, "Padosuso Input"s);
  m_up_midiin->ignoreTypes(false, false, false);
}

unsigned Statek::get_midi_count() const {
  return m_up_midiin->getPortCount();
}

std::string Statek::get_midi_name(unsigned idx) const {
  return m_up_midiin->getPortName(idx);
}

void Statek::init_midi() {
  m_up_midiin = std::make_unique<RtMidiIn>(
    RtMidi::Api::UNSPECIFIED,
    "Padosuso"s,
    150);
}

void Statek::emit(int type, int code, int val) {
  if (m_fd < 0)
    throw std::logic_error("cannot emit event because file descriptor "
      "is empty");

  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;
  /* timestamp values below are ignored */
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;

  write(m_fd, &ie, sizeof(ie));
}


bool Statek::loop() {
  double stamp = m_up_midiin->getMessage(&m_message);
  (void)stamp;
  if (m_message.size() >= 3) {
    int type_chyba = (int)m_message[0];
    int note = (int)m_message[1];
    int velocity = (int)m_message[2];
    (void)velocity;
    if (m_p_map->classic_keys.contains(note)) {
      if (type_chyba == NOTE_ON) {
        emit(EV_KEY, m_p_map->classic_keys.at(note), 1);
        emit(EV_SYN, SYN_REPORT, 0);
      } else if (type_chyba == NOTE_OFF) {
        emit(EV_KEY, m_p_map->classic_keys.at(note), 0);
        emit(EV_SYN, SYN_REPORT, 0);
      }
    } else {
      for (auto it = m_p_map->two_way_keys.cbegin();
          it != m_p_map->two_way_keys.cend();
          ++it) {
        if (type_chyba == NOTE_ON) {
          if (note == it->negative_note) {
            m_key_states[it->key] -= 1;
            emit(EV_ABS, it->key, m_key_states[it->key]);
            emit(EV_SYN, SYN_REPORT, 0);
          } else if (note == it->positive_note) {
            m_key_states[it->key] += 1;
            emit(EV_ABS, it->key, m_key_states[it->key]);
            emit(EV_SYN, SYN_REPORT, 0);
          }
        } else if (type_chyba == NOTE_OFF) {
          if (note == it->negative_note) {
            m_key_states[it->key] += 1;
            emit(EV_ABS, it->key, m_key_states[it->key]);
            emit(EV_SYN, SYN_REPORT, 0);
          } else if (note == it->positive_note) {
            m_key_states[it->key] -= 1;
            emit(EV_ABS, it->key, m_key_states[it->key]);
            emit(EV_SYN, SYN_REPORT, 0);
          }
        }
      }
    }
  }
  usleep(20);
  return not_end;
}

static void signal_handler(int s) {
  *p_not_end = false;
}

void Statek::occupy_sigint() {
  p_not_end = &not_end;
  std::signal(SIGINT, &signal_handler);
}

}