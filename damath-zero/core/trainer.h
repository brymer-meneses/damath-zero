#pragma once

#include <chrono>
#include <thread>

#include "damath-zero/core/actor.h"
#include "damath-zero/core/board.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Core {

template <Concepts::Board Board, Concepts::Network Network>
class Trainer {
  using Game = Game<Board>;
  using Actor = Actor<Board, Network>;

 public:
  constexpr Trainer(Config config) : config(config), replay_buffer(config) {}

  auto train() -> void;

 private:
  auto run_selfplay() -> void;
  auto play_game(Concepts::Network auto& network) -> void;
  auto train_network() -> void;

 public:
  Config config;
  NetworkStorage<Network> networks;
  ReplayBuffer<Board> replay_buffer;
};

template <Concepts::Board Board, Concepts::Network Network>
auto Trainer<Board, Network>::train() -> void {
  std::vector<std::thread> threads;
  std::atomic<bool> stop;

  for (auto i = 0; i < config.num_actors; i++) {
    threads.emplace_back([this, &stop]() {
      Actor actor(replay_buffer, networks, config, stop);
      actor.run();
    });
  }

  train_network();
  stop = true;
}

template <Concepts::Board Board, Concepts::Network Network>
auto Trainer<Board, Network>::train_network() -> void {
  while (replay_buffer.size() < config.batch_size) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::shared_ptr<Network> network = std::make_shared<Network>();

  torch::optim::Adam optimizer(network->parameters());
  torch::optim::ReduceLROnPlateauScheduler scheduler(
      optimizer, torch::optim::ReduceLROnPlateauScheduler::min, 0.5, 5, 0.0001,
      torch::optim::ReduceLROnPlateauScheduler::rel, 0, {0.0}, 1e-08, true);

  auto value_criterion = torch::nn::MSELoss();
  auto policy_criterion = torch::nn::CrossEntropyLoss();

  auto device = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;

  network->to(device);
  value_criterion->to(device);
  policy_criterion->to(device);

  for (auto i = 1; i < config.training_steps; i++) {
    if (i % config.checkpoint_interval == 0)
      networks.save(i, network);

    network->train();
    auto train_loss = torch::tensor(0.0);

    auto [input_features, target_values, target_policies] =
        replay_buffer.sample_batch();
    auto [values, policies] = network->forward(input_features);

    assert(values.dim() == target_values.dim());
    assert(policies.dim() == target_policies.dim());

    auto loss = value_criterion(values, target_values) +
                policy_criterion(policies, target_policies);

    train_loss += loss;

    optimizer.zero_grad();
    loss.backward();
    optimizer.step();

    scheduler.step(train_loss.item<f64>());

    std::println("Epoch {}: Train Loss = {}", i, train_loss.item<f64>());
  }
  networks.save(config.training_steps, network);
}

}  // namespace DamathZero::Core
