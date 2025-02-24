#ifndef DAMATH_ZERO_CORE_NETWORK_H
#define DAMATH_ZERO_CORE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/types.h"

template <typename N>
concept Network = requires(N n, torch::Tensor t) {
  { n.inference(t) } -> std::same_as<std::pair<f64, torch::Tensor>>;
};

#endif  // !DAMATH_ZERO_CORE_NETWORK_H
