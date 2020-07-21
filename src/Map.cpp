#include "Map.hpp"

#include <linux/input-event-codes.h>

namespace padosuso {

const Map quake_wsad { {
  { 43, KEY_W },
  { 45, KEY_S },
  { 41, KEY_A },
  { 47, KEY_D },
  { 48, KEY_SPACE },
  { 40, KEY_LEFTSHIFT },
  { 60, KEY_F13 },
  { 61, KEY_F14 },
  { 62, KEY_F15 },
  { 63, KEY_F16 },
  { 64, KEY_F17 },
  { 65, KEY_F18 },
  { 66, KEY_F19 },
  { 67, KEY_F20 },
  { 68, KEY_F21 },
  { 69, KEY_F22 },
  { 70, KEY_F23 },
  { 71, KEY_F24 },
} };

const Map csgo_wsad { {
  { 43, KEY_W },
  { 45, KEY_S },
  { 41, KEY_A },
  { 47, KEY_D },
  { 48, KEY_SPACE },
  { 40, KEY_LEFTSHIFT },
} };

const Map tekken_pad { {

  { 67, BTN_WEST },
  { 69, BTN_SOUTH },
  { 65, BTN_NORTH },
  { 71, BTN_EAST },


  { 46, BTN_TL },
  { 66, BTN_TR },

  { 54, BTN_SELECT },
  { 56, BTN_MODE },
  { 58, BTN_START },
}, {
  { 41, 47, ABS_HAT0X, -1, 1 },
  { 43, 45, ABS_HAT0Y, -1, 1 },
} };

}