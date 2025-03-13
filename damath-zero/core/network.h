#pragma once

#include <torch/torch.h>

#include "damath-zero/base/id.h"
#include "damath-zero/base/macros.h"

namespace DamathZero::Core {

namespace Concepts {

template <typename N>
concept Network =
    std::is_base_of_v<torch::nn::Module, N> && requires(N n, torch::Tensor t) {
      { n.forward(t) } -> std::same_as<std::pair<torch::Tensor, torch::Tensor>>;
    };

}  // namespace Concepts

struct CheckpointId : Base::Id {
  using Id::Id;
};

struct UniformNetwork : torch::nn::Module {
  auto forward(torch::Tensor x) -> std::pair<torch::Tensor, torch::Tensor> {
    return {torch::tensor({0.5}), torch::softmax(torch::ones_like(x), -1)};
  }
};

REQUIRE_CONCEPT(Concepts::Network, UniformNetwork);

template <Concepts::Network Network>
class NetworkStorage {
 public:
  auto get_latest() -> Network&;
  auto save(CheckpointId id, Network network) -> void;

  constexpr auto empty() const -> bool { return networks_.empty(); }

 private:
  mutable std::mutex mutex_;
  std::vector<std::pair<CheckpointId, Network>> networks_;
};

template <Concepts::Network Network>
auto NetworkStorage<Network>::get_latest() -> Network& {
  assert(not networks_.empty());

  std::lock_guard lock(mutex_);
  return networks_.back().second;
}

template <Concepts::Network Network>
auto NetworkStorage<Network>::save(CheckpointId id, Network network) -> void {}

}  // namespace DamathZero::Core
