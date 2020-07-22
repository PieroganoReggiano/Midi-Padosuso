#pragma once

#include <memory>
#include <linux/uinput.h>
#include "RtMidi.h"

#include "consts.hpp"
#include "Map.hpp"

namespace padosuso {

class Statek {
 public:

  static inline constexpr const Map *P_DEFAULT_MAP = &quake_wsad;

  Statek();
  ~Statek();

  Statek(const Statek&) = delete;

  void activate_map(const Map*);

  void use_midi(unsigned);

  unsigned get_midi_count() const;

  std::string get_midi_name(unsigned) const;

  const Map *get_map() const {
    return m_p_map;
  }

  bool pad_activated() const {
    return m_p_map && m_fd >= 0;
  }

  bool midi_activated() const {
    return m_up_midiin != nullptr;
  }

  bool loop();

  static void occupy_sigint();

 private:
  int m_fd;
  std::unique_ptr<RtMidiIn> m_up_midiin;
  std::vector<unsigned char> m_message;
  
  /// for keys other than 0/1
  std::map<int, int> m_abs_states;

  /// Empty if pad is off.
  const Map *m_p_map;

  volatile inline static bool not_end = true;

  void deactivate_map_and_uinput();
  void init_midi();
  void emit(int type, int code, int val);

};

}