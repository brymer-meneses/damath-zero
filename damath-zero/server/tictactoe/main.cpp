
#include "damath-zero/server/tictactoe/server.h"

auto main(int argc, char** argv) -> int {
  DamathZero::Games::TicTacToe::Server server;
  server.start();
  return 0;
}
