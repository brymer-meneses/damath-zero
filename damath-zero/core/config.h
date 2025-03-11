#pragma once

#include "damath-zero/base/types.h"

namespace DamathZero::Core {

struct Config {
  // PUCT formula
  f64 c_base = 19652;
  f64 c_init = 1.25;

  i32 num_simulations = 100;
  i32 max_moves = 100;

  i32 num_actors = 6;

  i32 buffer_size = 10000;
  i32 batch_size = 32;

  i32 training_steps = 10000000;
  i32 checkpoint_interval = 1000;
};

}  // namespace DamathZero::Core
