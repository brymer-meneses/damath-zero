#pragma once

#include <torch/torch.h>

#include <format>

#include "damath-zero/base/id.h"
#include "damath-zero/base/macros.h"

namespace DamathZero::Core {

struct CheckpointId : Base::Id {
  using Id::Id;
};

template <typename N>
concept Network =
    requires(N n, torch::Tensor t) { std::is_base_of_v<torch::nn::Module, N>; };

struct UniformNetwork : public torch::nn::Module {
 public:
  auto forward(torch::Tensor input)
      -> std::tuple<torch::Tensor, torch::Tensor> {
    return {torch::tensor({0.5}), torch::softmax(torch::ones_like(input), 0)};
  }
};

REQUIRE_CONCEPT(Network, UniformNetwork);

template <Network Network>
class NetworkStorage {
 public:
  auto get_latest() -> Network&;
  auto save(CheckpointId id, Network network) -> void;

  constexpr auto empty() const -> bool { return networks_.empty(); }

 private:
  mutable std::mutex mutex_;
  std::vector<std::pair<CheckpointId, Network>> networks_;
};

template <Network Network>
auto NetworkStorage<Network>::get_latest() -> Network& {
  std::lock_guard lock(mutex_);
  return networks_.back().second;
}

template <Network Network>
auto NetworkStorage<Network>::save(CheckpointId id, Network network) -> void {}

}  // namespace DamathZero::Core
