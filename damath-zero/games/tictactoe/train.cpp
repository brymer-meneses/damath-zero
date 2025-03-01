#include "damath-zero/base/types.h"
#include "damath-zero/core/context.h"
#include "damath-zero/games/tictactoe/game.h"
#include "damath-zero/games/tictactoe/network.h"

using namespace DamathZero::Games;
using namespace DamathZero;

auto main() -> i32 {
  Core::Config config;
  Core::Context<TicTacToe::Game, TicTacToe::Network> context(config);

  context.run();

  return 0;
}
