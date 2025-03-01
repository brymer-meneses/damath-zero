#ifndef DAMATH_ZERO_CORE_NETWORK_H
#define DAMATH_ZERO_CORE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/types.h"

namespace DamathZero::Core {

template <typename N>
concept Network = requires(N n, torch::Tensor t) {
  { n.inference(t) } -> std::same_as<torch::Tensor>;
  { n.save() } -> std::same_as<void>;
};

// TODO: this should use mutexes to avoid data races
template <Network Network>
class NetworkStorage {
 public:
  auto get_latest() const -> Network&;
  auto save(u32 id, Network network) -> void;

 private:
  std::vector<std::pair<u32, Network>> networks_;
};

template <Network Network>
auto NetworkStorage<Network>::get_latest() const -> Network& {
  if (networks_.size() > 0) {
  }

  return networks_.back();
}

template <Network Network>
auto NetworkStorage<Network>::save(u32 id, Network network) -> void {
  networks_.push_back({id, network});
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_NETWORK_H
