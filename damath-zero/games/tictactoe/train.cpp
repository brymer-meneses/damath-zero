#include "damath-zero/base/types.h"
#include "damath-zero/core/context.h"
#include "damath-zero/games/tictactoe/game.h"
#include "damath-zero/games/tictactoe/network.h"

using namespace DamathZero;

auto main() -> i32 {
  Core::Config config;
  Core::Context<Games::TicTacToe, Games::TicTacToeNetwork> context(config);

  context.run();

  return 0;
}
