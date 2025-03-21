#pragma once

#include <torch/torch.h>

#include "damath-zero/base/id.h"
#include "damath-zero/base/macros.h"

namespace DamathZero::Core {

namespace Concepts {

template <typename N>
concept Network =
    std::is_base_of_v<torch::nn::Module, N> and requires(N n, torch::Tensor t) {
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
  auto get_latest() -> std::shared_ptr<Network>;
  auto save(CheckpointId id, std::shared_ptr<Network> network) -> void;

  constexpr auto empty() const -> bool { return networks_.empty(); }

 private:
  mutable std::mutex mutex_;
  std::vector<std::pair<CheckpointId, std::shared_ptr<Network>>> networks_;
};

template <Concepts::Network Network>
auto NetworkStorage<Network>::get_latest() -> std::shared_ptr<Network> {
  assert(not networks_.empty());

  std::lock_guard lock(mutex_);
  return networks_.back().second;
}

template <Concepts::Network Network>
auto NetworkStorage<Network>::save(CheckpointId id, std::shared_ptr<Network> network) -> void {
  const auto checkpoint_path = std::format("models/model_{}.pt", id.value());

  torch::serialize::OutputArchive output_model_archive;
  network->to(torch::kCPU);
  network->save(output_model_archive);
  output_model_archive.save_to(checkpoint_path);

  // Load model state
  torch::serialize::InputArchive input_archive;
  input_archive.load_from(checkpoint_path);

  auto new_network = std::make_shared<Network>();
  new_network->load(input_archive);
  new_network->to(torch::kCPU);

  networks_.emplace_back(id, new_network);
}

}  // namespace DamathZero::Core
