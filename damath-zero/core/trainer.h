#pragma once

#include <chrono>
#include <print>
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
  auto play_game(Network& network) -> void;
  auto update_weights(Network& network, torch::optim::Optimizer& optimizer)
      -> void;
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
auto Trainer<Board, Network>::play_game(Network& network) -> void {
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
    auto network = networks.get_latest();
    play_game(network);
  }
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::train_network() -> void {
  while (replay_buffer.size() < config.batch_size) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  Network network;
  torch::optim::SGD optimizer(network.parameters(),
                              torch::optim::SGDOptions(0.2));

  for (auto i = 0; i < config.training_steps; i++) {
    if (i % config.checkpoint_interval == 0)
      networks.save(i, network);
    update_weights(network, optimizer);
  }
  networks.save(config.training_steps, std::move(network));
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::update_weights(Network& network,
                                             torch::optim::Optimizer& optimizer)
    -> void {
  optimizer.zero_grad();
  auto [input_features, target_values, target_policies] =
      replay_buffer.sample_batch();
  auto [values, policies] = network.forward(input_features);
  auto loss = torch::mse_loss(values, target_values) +
              torch::nn::functional::cross_entropy(policies, target_policies);

  AT_ASSERT(!std::isnan(loss.template item<f64>()));

  loss.backward();
  optimizer.step();

  std::println("Loss: {}", loss.template item<f64>());
}

}  // namespace DamathZero::Core
