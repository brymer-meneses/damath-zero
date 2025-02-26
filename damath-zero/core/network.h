#ifndef DAMATH_ZERO_CORE_NETWORK_H
#define DAMATH_ZERO_CORE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/types.h"

namespace DamathZero::Core {

template <typename N>
concept Network = requires(N n, torch::Tensor t) {
  { n.forward(t) } -> std::same_as<std::pair<f64, torch::Tensor>>;
};

template <Network Network>
class NetworkStorage {
 public:
  auto get_latest_network() const -> Network&;
  auto save_network() -> void;

 private:
  std::vector<Network> networks_;
};

template <Network Network>
auto NetworkStorage<Network>::get_latest_network() const -> Network& {
  if (networks_.size() > 0) {
  }

  return networks_.back();
}

template <Network Network>
auto NetworkStorage<Network>::save_network() -> void {}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_NETWORK_H
