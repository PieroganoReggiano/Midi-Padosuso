#include <linux/uinput.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <optional>
#include <csignal>
#include <memory>
#include <vector>

#include "RtMidi.h"

#include "consts.hpp"
#include "Map.hpp"

#include "Statek.hpp"


int main(int argc, char** argv) {

  using namespace padosuso;

  Statek statek;

  Statek::occupy_sigint();

  //statek.activate_map(Statek::P_DEFAULT_MAP);
  statek.activate_map(&tekken_pad);

  // Here program will fail if there is not midi at 1.
  // TODO make some nice choosing of devices.
  statek.use_midi(1);
  // More, after initialization this can be changed by some ALSA tools or even
  // by JACK Audio patchbays.

  while(statek.loop());

  return 0;
}