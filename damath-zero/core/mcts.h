#ifndef DAMATH_ZERO_CORE_MCTS_H
#define DAMATH_ZERO_CORE_MCTS_H

#include <cassert>
#include <span>

#include "damath-zero/core/game.h"
#include "damath-zero/core/network.h"
#include "damath-zero/core/node.h"

namespace DamathZero::Core {

struct Config {
  f64 c_base;
  f64 c_init;

  i32 num_simulations;
};

class MCTS {
 public:
  MCTS(Config config) : config_(config) {}

  template <Game Game, Network Network>
  auto run(Game game, Network network) -> NodeId;

 private:
  auto select_highest_puct_score(NodeId parent) const -> NodeId;
  auto select_highest_visit_count(NodeId parent) const -> NodeId;
  auto compute_puct_score(NodeId parent, NodeId child) const -> f64;

  auto backpropagate(std::span<NodeId> path, f64 value, Player player) -> void;

  template <Game Game, Network Network>
  auto expand_node(NodeId node, Game game, Network network) -> f64;

 private:
  NodeStorage nodes_;
  Config config_;
};

template <Game Game, Network Network>
auto MCTS::run(Game game, Network network) -> NodeId {
  auto root_id = nodes_.create(ActionId::invalid(), 0);
  auto _ = expand_node(root_id, game, network);

  for (auto i = 0; i < config_.num_simulations; i++) {
    std::vector<NodeId> path;

    auto node_id = root_id;
    auto& node = nodes_.get(node_id);
    auto scratch_game = game.clone();

    while (node.is_expanded()) {
      node_id = select_highest_puct_score(node_id);
      auto& node = nodes_.get(node_id);
      auto action_id = node.action_taken;
      scratch_game.apply(action_id);
      path.push_back(node_id);
    }

    auto value = expand_node(node_id, scratch_game, network);
    backpropagate(path, value, scratch_game.get_current_player());
  }

  return select_highest_visit_count(root_id);
}

template <Game Game, Network Network>
auto MCTS::expand_node(NodeId node_id, Game game, Network network) -> f64 {
  auto [value, policy] = network.inference(game.make_image());
  auto& node = nodes_.get(node_id);
  node.played_by = game.get_current_player();

  auto legal_actions = game.get_legal_actions();

  return 0;
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_MCTS_H
