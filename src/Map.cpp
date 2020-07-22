#include "Map.hpp"

#include <linux/input-event-codes.h>

namespace padosuso {

const Map quake_wsad {
  /* ev_keys */ {
    KEY_W,
    KEY_S,
    KEY_A,
    KEY_D,
    KEY_SPACE,
    KEY_LEFTSHIFT,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24,
  },
  /* ev_abss */ {},
  /* classic_keys */ {
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
  },
  /* two_way_keys */ {},
};

const Map csgo_wsad { {
  // TODO
} };

const Map tekken_pad {
  /* ev_keys */ {
    BTN_WEST,
    BTN_SOUTH,
    BTN_NORTH,
    BTN_EAST,
    BTN_TL,
    BTN_TR,
    BTN_SELECT,
    BTN_MODE,
    BTN_START,
    BTN_THUMBL,
    BTN_THUMBR,
  },
  /* ev_abss */ {
    { ABS_X, { -32768, 32767, 16, 128 }},
    { ABS_Y, { -32768, 32767, 16, 128 }},
    { ABS_Z, { 0, 1023 }},

    { ABS_RX, { -32768, 32767, 16, 128 }},
    { ABS_RY, { -32768, 32767, 16, 128 }},
    { ABS_RZ, { 0, 1023 }},

    { ABS_HAT0X, { -1, 1 }},
    { ABS_HAT0Y, { -1, 1 }},
  },
  /* classic_keys */ {
    { 67, BTN_WEST },
    { 69, BTN_SOUTH },
    { 65, BTN_NORTH },
    { 71, BTN_EAST },

    { 46, BTN_TL },
    { 66, BTN_TR },

    { 54, BTN_SELECT },
    { 56, BTN_MODE },
    { 58, BTN_START },
  },
  /* two_way_keys */ {
    { 41, { ABS_HAT0X, -1 }},
    { 47, { ABS_HAT0X, 1 }},
    { 43, { ABS_HAT0Y, -1 }},
    { 45, { ABS_HAT0Y, 1 }},

    { 44, { ABS_Z, 1023 }},
    { 68, { ABS_RZ, 1023 }},
  },
};

}