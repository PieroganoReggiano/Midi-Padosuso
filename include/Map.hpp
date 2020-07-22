#pragma once

#include <set>
#include <map>
#include <vector>

namespace padosuso {

using Note = unsigned char;

struct ClassicKey {
  unsigned char note;
  int key;
  constexpr ClassicKey(std::pair<decltype(note), decltype(key)> q)
    : note(q.first), key(q.second) {}
};

struct TwoWayKey {
  int abs_code;
  int added_value;
};

struct AbsProps {
  int min = -1;
  int max = 1;
  int fuzz = 0;
  int flat = 0;
};

struct Map {
  std::set<int> ev_keys;
  std::map<int, AbsProps> ev_abss;

  /// Key must be added in 'ev_keys'.
  std::map<Note, int> classic_keys;

  /// Abses
  std::map<Note, TwoWayKey> two_way_keys;
};

extern const Map quake_wsad;
extern const Map csgo_wsad;
extern const Map tekken_pad;

}