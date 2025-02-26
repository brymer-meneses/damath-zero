#ifndef DAMATH_ZERO_CORE_CONFIG_H
#define DAMATH_ZERO_CORE_CONFIG_H

#include "damath-zero/base/types.h"

namespace DamathZero::Core {

struct Config {
  // PUCT formula
  f64 c_base;
  f64 c_init;

  i32 num_simulations;
  i32 max_moves;

  i32 num_actors;

  i32 buffer_size;
  i32 batch_size;

  i32 training_steps;
  i32 checkpoint_interval;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_CONFIG_H
