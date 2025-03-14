#pragma once

#include <algorithm>
#include <memory>
#include <print>

#include "damath-zero/core/board.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Core {

template <Concepts::Board Board, Concepts::Network Network>
class Actor {
 public:
  constexpr Actor(ReplayBuffer<Board>& replay_buffer,
                  NetworkStorage<Network>& networks, Config config,
                  std::atomic<bool>& should_terminate)
      : replay_buffer_(replay_buffer),
        networks_(networks),
        config_(config),
        mcts_(config),
        should_terminate_(should_terminate) {}

  auto run() -> void;

 private:
  template <Concepts::Network AnyNetwork>
  auto generate_self_play_data(std::shared_ptr<AnyNetwork> network) -> void;

 private:
  NetworkStorage<Network>& networks_;
  ReplayBuffer<Board>& replay_buffer_;
  std::atomic<bool>& should_terminate_;
  Config config_;
  MCTS mcts_;
};

template <Concepts::Board Board, Concepts::Network Network>
template <Concepts::Network AnyNetwork>
auto Actor<Board, Network>::generate_self_play_data(
    std::shared_ptr<AnyNetwork> network) -> void {

  Game<Board> game;
  while (not game.is_terminal() and game.history_size() < config_.max_moves) {
    mcts_.reset();

    auto player = game.to_play();
    auto root_id = mcts_.run(player, game.board(), game.history_size(), network);
    auto action = mcts_.nodes().get(root_id).action_taken;

    game.apply(action);
    game.store_search_statistics(mcts_.nodes(), root_id);
  }
  replay_buffer_.save_game(std::move(game));
}

template <Concepts::Board Board, Concepts::Network Network>
auto Actor<Board, Network>::run() -> void {
  while (not should_terminate_) {
    if (networks_.empty()) {
      generate_self_play_data(std::make_shared<UniformNetwork>());
      continue;
    }

    generate_self_play_data(networks_.get_latest());
  }
}

}  // namespace DamathZero::Core
