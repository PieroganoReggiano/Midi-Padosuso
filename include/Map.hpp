#pragma once

#include <map>
#include <vector>

namespace padosuso {

struct ClassicKey {
  unsigned char note;
  int key;
  constexpr ClassicKey(std::pair<decltype(note), decltype(key)> q)
    : note(q.first), key(q.second) {}
};

struct TwoWayKey {
  unsigned char negative_note;
  unsigned char positive_note;
  int key;
  int min;
  int max;
};

struct Map {
  std::map<unsigned char, int> classic_keys;
  std::vector<TwoWayKey> two_way_keys;
};

extern const Map quake_wsad;
extern const Map csgo_wsad;
extern const Map tekken_pad;

}