#ifndef DAMATH_ZERO_CORE_CONTEXT_H
#define DAMATH_ZERO_CORE_CONTEXT_H

#include <thread>

#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Core {

template <Game Game, Network Network>
class Context {
 public:
  Context(Config config) : config_(config) {}
  auto run() -> void;

 private:
  auto run_selfplay() -> void;
  auto play_game(Game game, Network network) -> void;
  auto train_network() -> void;

 private:
  Config config_;
  NetworkStorage<Network> networks_;
  ReplayBuffer<Game> replay_buffer_;
};

template <Game Game, Network Network>
auto Context<Game, Network>::run() -> void {
  std::vector<std::thread> threads;

  for (auto i = 0; i < config_.num_actors; i++) {
    threads.push_back(std::thread(run_selfplay));
  }

  // NOTE: we do not need to join the threads since we want the process to stop
  // after `config.training_steps` which is controlled in `train_network`
  // running on the main thread.
  train_network();
}

template <Game Game, Network Network>
auto Context<Game, Network>::play_game(Game game, Network network) -> void {
  while (not game.is_terminal() and
         game.get_history().size() < config_.max_moves) {
    auto mcts = MCTS(config_);
    auto root_id = mcts.run(game, network);
    auto action_id = mcts.nodes().get(root_id).action_id;
    game.apply(action_id);
  }
}

template <Game Game, Network Network>
auto Context<Game, Network>::run_selfplay() -> void {
  while (true) {
    Game game;
    auto network = networks_.get_latest();
    play_game(game, network);
  }
}

template <Game Game, Network Network>
auto Context<Game, Network>::train_network() -> void {
  auto network = Network();

  for (auto i = 0; i < config_.training_steps; i++) {
    if (i % config_.checkpoint_interval == 0)
      networks_.save(i, network);
    auto batch = replay_buffer_.sample_batch();
  }
  networks_.save(config_.training_steps, network);
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_CONTEXT_H
