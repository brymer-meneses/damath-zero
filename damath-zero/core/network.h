#ifndef DAMATH_ZERO_CORE_NETWORK_H
#define DAMATH_ZERO_CORE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/id.h"

namespace DamathZero::Core {

struct CheckpointId : Base::Id {
  using Id::Id;
};

template <typename N>
concept Network =
    requires(N n, torch::Tensor t) { std::is_base_of_v<torch::nn::Module, N>; };

template <Network Network>
class NetworkStorage {
 public:
  auto get_latest() -> Network&;
  auto save(CheckpointId id, Network network) -> void;

 private:
  mutable std::mutex mutex_;
  std::vector<std::pair<CheckpointId, Network>> networks_;
};

template <Network Network>
auto NetworkStorage<Network>::get_latest() -> Network& {
  std::lock_guard lock(mutex_);

  if (networks_.empty()) {
    networks_.push_back(Network());
  }
  return networks_.back().second;
}

template <Network Network>
auto NetworkStorage<Network>::save(CheckpointId id, Network network) -> void {
  std::lock_guard lock(mutex_);

  networks_.push_back({id, network});
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_NETWORK_H
