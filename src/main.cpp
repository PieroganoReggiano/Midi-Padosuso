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

  statek.use_midi(1);

  while(statek.loop());

  return 0;
}