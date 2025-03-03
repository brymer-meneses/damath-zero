#ifndef DAMATH_ZERO_CORE_TRAINER_H
#define DAMATH_ZERO_CORE_TRAINER_H

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
  Trainer(Config config) : config_(config), replay_buffer_(config) {}
  auto train() -> void;

 private:
  auto run_selfplay() -> void;
  auto play_game(Network network) -> void;
  auto update_weights(Network network) -> void;
  auto train_network() -> void;

 private:
  Config config_;
  NetworkStorage<Network> networks_;
  ReplayBuffer<Board> replay_buffer_;
};

template <Board Board, Network Network>
auto Trainer<Board, Network>::train() -> void {
  std::vector<std::thread> threads;

  for (auto i = 0; i < config_.num_actors; i++) {
    threads.push_back(
        std::thread(&Trainer<Board, Network>::run_selfplay, this));
  }

  // NOTE: we do not need to join the threads since we want the process to stop
  // after `config.training_steps` which is controlled in `train_network`
  // running on the main thread.
  train_network();
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::play_game(Network network) -> void {
  Game game;
  while (not game.is_terminal() and game.history_size() < config_.max_moves) {
    auto mcts = MCTS(config_);
    auto player = game.to_play();
    auto root_id = mcts.run(player, game.board(), network);
    auto action = mcts.nodes().get(root_id).action_taken;

    game.apply(action);
    game.store_search_statistics(mcts.nodes(), root_id);
  }

  replay_buffer_.save_game(std::move(game));
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::run_selfplay() -> void {
  while (true) {
    auto network = networks_.get_latest();
    play_game(network);
  }
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::train_network() -> void {
  auto network = Network();

  for (auto i = 0; i < config_.training_steps; i++) {
    if (i % config_.checkpoint_interval == 0)
      networks_.save(i, network);
    update_weights(network);
  }
  networks_.save(config_.training_steps, network);
}

template <Board Board, Network Network>
auto Trainer<Board, Network>::update_weights(Network network) -> void {
  auto batch = replay_buffer_.sample_batch();
  auto loss = 0.0;

  for (auto [feature, target] : batch) {
    auto [value, policy] = network.forward(feature);
  }
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_TRAINER_H
