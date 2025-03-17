#include "damath-zero/core/trainer.h"
#include "damath-zero/games/connect2/board.h"
#include "damath-zero/games/connect2/network.h"

using namespace DamathZero::Games;
using namespace DamathZero;

auto main() -> i32 {
  Core::Config config;
  Core::Trainer<Connect2::Board, Connect2::Network> trainer(config);

  trainer.train();

  return 0;
}
