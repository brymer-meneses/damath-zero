#pragma once

#include "damath-zero/base/types.h"

namespace DamathZero::Core {

struct Config {
  // PUCT formula
  f64 c_base = 19652;
  f64 c_init = 1.25;

  f64 root_dirichlet_alpha = 0.3;
  f64 root_exploration_fraction = 0.25;

  i32 num_sampling_moves = 3;
  i32 num_simulations = 100;
  i32 max_moves = 100;

  i32 num_actors = 6;

  i32 buffer_size = 10000;
  i32 batch_size = 256;

  i32 training_steps = 100000000;
  i32 checkpoint_interval = 1000;
};

}  // namespace DamathZero::Core
