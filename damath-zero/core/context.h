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

 private:
  Config config_;
  NetworkStorage<Network> networks_;
};

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
  for (auto i = 0; i < config_.num_games_per_actor; i++) {
    Game game;
    auto network = networks_.get_latest_network();
    play_game(game, network);
  }
}

template <Game Game, Network Network>
auto Context<Game, Network>::run() -> void {
  std::vector<std::thread> threads;

  for (auto i = 0; i < config_.num_actors; i++) {
    threads.emplace_back(std::thread(run_selfplay));
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_CONTEXT_H
