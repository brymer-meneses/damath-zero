#include "damath-zero/core/trainer.h"
#include "damath-zero/games/tictactoe/board.h"
#include "damath-zero/games/tictactoe/network.h"

using namespace DamathZero::Games;
using namespace DamathZero;

auto main() -> i32 {
  Core::Config config;
  Core::Trainer<TicTacToe::Board, TicTacToe::Network> trainer(config);

  trainer.train();

  return 0;
}
