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

inline static constexpr bool use_xones_numbers = true;

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

  struct uinput_setup usetup;
  std::memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;

  usetup.id.vendor = 0x00ff;
  usetup.id.product = 0x0000;
  usetup.id.version = 0x0001;
  
  if (use_xones_numbers) {
    // Here we set Microsoft X-Box One S pad numbers so many programs will
    // deal with Padosuso like with X-Box One gamepad :>
    usetup.id.vendor = 0x045e;
    usetup.id.product = 0x02ea;
    usetup.id.version = 0x0301;
  }
  std::strcpy(usetup.name, "Padosuso");

  // Normal buttons (EV_KEY)
  if (!p_map->ev_keys.empty()) {
    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
    for (auto it = p_map->ev_keys.cbegin(); it != p_map->ev_keys.cend(); ++it) {
      ioctl(m_fd, UI_SET_KEYBIT, *it);
    }
  }
  
  // Absolute buttons (EV_ABS)
  if (!p_map->ev_abss.empty()) {
    ioctl(m_fd, UI_SET_EVBIT, EV_ABS);
    for (auto it = p_map->ev_abss.cbegin(); it != p_map->ev_abss.cend(); ++it) {
      int abs_code = it->first;
      ioctl(m_fd, UI_SET_ABSBIT, abs_code);
      m_abs_states[abs_code] = 0;
      struct uinput_abs_setup abs_setup;
      std::memset(&abs_setup, 0, sizeof(abs_setup));
      abs_setup.code = abs_code;
      abs_setup.absinfo.minimum = it->second.min;
      abs_setup.absinfo.maximum = it->second.max;
      abs_setup.absinfo.fuzz = it->second.fuzz;
      abs_setup.absinfo.flat = it->second.flat;
      ioctl(m_fd, UI_ABS_SETUP, &abs_setup);
    }
  }

  ioctl(m_fd, UI_DEV_SETUP, &usetup);
  ioctl(m_fd, UI_DEV_CREATE);

  m_p_map = p_map;
  usleep(100);
}

void Statek::deactivate_map_and_uinput() {
  if (m_fd >= 0) {
    ioctl(m_fd, UI_DEV_DESTROY);
    close(m_fd);
  }
  m_abs_states.clear();
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
    int event_type = m_message[0];
    Note note = m_message[1];
    int velocity = m_message[2];
    (void)velocity;
    // Here, classic keys have priority over abs, so if one is in
    // both sets, only classic will be handled.
    // Also, by now we handle only NOTE_ON (144) and NOTE_OFF events.
    decltype(Map::classic_keys)::const_iterator ckit;
    decltype(Map::two_way_keys)::const_iterator twkit;
    if (event_type == NOTE_ON) {
      if ((ckit = m_p_map->classic_keys.find(note))
          != m_p_map->classic_keys.cend()) {
        emit(EV_KEY, ckit->second, 1);
        emit(EV_SYN, SYN_REPORT, 0);
      } else if ((twkit = m_p_map->two_way_keys.find(note))
          != m_p_map->two_way_keys.cend()) {
        int value =
          m_abs_states[twkit->second.abs_code] + twkit->second.added_value;
        m_abs_states[twkit->second.abs_code] = value;
        emit(EV_ABS, twkit->second.abs_code, value);
        emit(EV_SYN, SYN_REPORT, 0);
      }
    } else if (event_type == NOTE_OFF) {
      if ((ckit = m_p_map->classic_keys.find(note))
          != m_p_map->classic_keys.cend()) {
        emit(EV_KEY, ckit->second, 0);
        emit(EV_SYN, SYN_REPORT, 0);
      } else if ((twkit = m_p_map->two_way_keys.find(note))
          != m_p_map->two_way_keys.cend()) {
        int value =
          m_abs_states[twkit->second.abs_code] - twkit->second.added_value;
        m_abs_states[twkit->second.abs_code] = value;
        emit(EV_ABS, twkit->second.abs_code, value);
        emit(EV_SYN, SYN_REPORT, 0);
      }
    }
  }
  usleep(60);
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