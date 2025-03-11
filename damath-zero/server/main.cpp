#include "damath-zero/server/tictactoe.h"

using namespace DamathZero::Games;

auto main(int argc, char** argv) -> int {
  TicTacToe::Server server;
  server.start();
  return 0;
}
