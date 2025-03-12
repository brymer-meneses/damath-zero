#pragma once

#include <chrono>
#include <thread>

#include "damath-zero/core/board.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Core {

template <Board Board, Network Network>
class Trainer {
  using Game = Game<Board>;

 public:
  Trainer(Config config) : config(config), replay_buffer(config) {}
  auto train() -> void;

 private:
  auto run_selfplay() -> void;
  auto play_game(Core::Network auto& network) -> void;
  auto train_network() -> void;

 public:
  Config config;
  NetworkStorage<Network> networks;
  ReplayBuffer<Board> replay_buffer;
};

template <Board Board, Network Network>
auto Trainer<Board, Network>::train() -> void {
  std::vector<std::thread> threads;

  for (auto i = 0; i < config.num_actors; i++) {
    threads.push_back(std::thread([this]() { run_selfplay(); }));
  }

  // NOTE: we do not need to join the threads since we want the process to stop
  // after `config.training_steps` which is controlled in `train_network`
  // running on the main thread.
  train_network();
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::play_game(Core::Network auto& network) -> void {
  Game game;
  while (not game.is_terminal() and game.history_size() < config.max_moves) {
    auto mcts = MCTS(config);
    auto player = game.to_play();
    auto root_id = mcts.run(player, game.board(), network);
    auto action = mcts.nodes().get(root_id).action_taken;

    game.apply(action);
    game.store_search_statistics(mcts.nodes(), root_id);
  }
  replay_buffer.save_game(std::move(game));
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::run_selfplay() -> void {
  while (true) {
    if (networks.empty()) {
        UniformNetwork network;
        play_game(network);
        continue;
    }

    auto& network = networks.get_latest();
    play_game(network);
  }
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::train_network() -> void {
  while (replay_buffer.size() < config.batch_size) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  Network network;

  torch::optim::Adam optimizer(network.parameters());
  torch::optim::ReduceLROnPlateauScheduler scheduler(
      optimizer, torch::optim::ReduceLROnPlateauScheduler::min, 0.5, 5, 0.0001,
      torch::optim::ReduceLROnPlateauScheduler::rel, 0, {0.0}, 1e-08, true);

  auto value_criterion = torch::nn::MSELoss();
  auto policy_criterion = torch::nn::CrossEntropyLoss();

  auto device = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;

  network.to(device);
  value_criterion->to(device);
  policy_criterion->to(device);

  for (auto i = 1; i < config.training_steps; i++) {
    if (i % config.checkpoint_interval == 0)
      networks.save(i, network);

    network.train();
    auto train_loss = torch::tensor(0.0);

    auto [input_features, target_values, target_policies] =
        replay_buffer.sample_batch();
    for (auto batch = 0; batch < config.batch_size; batch++) {
      auto [value, policy] = network.forward(input_features[batch]);

      auto loss = value_criterion(value, target_values[batch]) +
                  policy_criterion(policy, target_policies[batch]);
      train_loss += loss;

      optimizer.zero_grad();
      loss.backward();
      optimizer.step();
    }
    train_loss /= config.batch_size;

    network.eval();
    auto eval_loss = torch::tensor(0.0);
    {
      torch::NoGradGuard guard;
      auto [input_features, target_values, target_policies] =
          replay_buffer.sample_batch();
      for (auto batch = 0; batch < config.batch_size; batch++) {
        auto [value, policy] = network.forward(input_features[batch]);

        auto loss = value_criterion(value, target_values[batch]) +
                    policy_criterion(policy, target_policies[batch]);
        eval_loss += loss;
      }
    }
    eval_loss /= config.batch_size;
    scheduler.step(eval_loss.item<double>());

    std::println("Epoch {}: Train Loss = {} | Eval Loss = {}", i, train_loss.item<f64>(), eval_loss.item<f64>());
  }
  networks.save(config.training_steps, network);
}

}  // namespace DamathZero::Core
