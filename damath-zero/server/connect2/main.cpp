
#include "damath-zero/server/connect2/server.h"

auto main(int argc, char** argv) -> int {
  DamathZero::Games::Connect2::Server server;
  server.start();
  return 0;
}
