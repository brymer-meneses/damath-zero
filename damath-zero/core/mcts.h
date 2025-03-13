#pragma once

#include <torch/torch.h>

#include <cassert>
#include <print>
#include <span>
#include <vector>

#include "damath-zero/core/config.h"
#include "damath-zero/core/network.h"
#include "damath-zero/core/node.h"

namespace DamathZero::Core {

class MCTS {
 public:
  MCTS(Config config) : config_(config) {}

  auto run(Player previous_player, Concepts::Board auto board,
           Concepts::Network auto network) -> NodeId;

  constexpr auto reset() -> void { nodes_.clear(); }

  constexpr auto nodes() const -> NodeStorage const& { return nodes_; }
  constexpr auto nodes() -> NodeStorage& { return nodes_; };

 private:
  auto select_highest_puct_score(NodeId parent) const -> NodeId;
  auto select_highest_visit_count(NodeId parent) const -> NodeId;
  auto compute_puct_score(NodeId parent, NodeId child) const -> f64;

  auto backpropagate(std::span<NodeId> path, f64 value, Player player) -> void;

  template <Concepts::Board Board, Concepts::Network Network>
  auto expand_node(NodeId node, Player player, Board board, Network network)
      -> f64;

 private:
  NodeStorage nodes_;
  Config config_;
};

auto MCTS::run(Player player, Concepts::Board auto board,
               Concepts::Network auto network) -> NodeId {
  torch::NoGradGuard guard;

  auto root_id = nodes_.create(0.0);
  auto _ = expand_node(root_id, player, board, network);
  assert(nodes_.get(root_id).is_expanded());

  for (auto i = 0; i < config_.num_simulations; i++) {
    std::vector<NodeId> path;

    auto node_id = root_id;

    while (nodes_.get(node_id).is_expanded()) {
      node_id = select_highest_puct_score(node_id);
      auto action_id = nodes_.get(node_id).action_taken;
      auto [new_player, new_board] = board.apply(player, action_id);
      board = new_board;
      player = new_player;

      path.push_back(node_id);
    }

    auto value = expand_node(node_id, player, board, network);
    backpropagate(path, value, player);
  }

  return select_highest_visit_count(root_id);
}

template <Concepts::Board Board, Concepts::Network Network>
auto MCTS::expand_node(NodeId node_id, Player player, Board board,
                       Network network) -> f64 {
  auto [value, policy] = network.forward(board.get_feature(player));
  auto legal_actions = board.get_legal_actions(player);

  // Create a boolean mask for legal actions
  auto legal_mask = torch::zeros(Board::ActionSize, torch::kBool);
  for (ActionId action : legal_actions) {
    legal_mask[action.value()] = true;
  }

  // Use where to mask illegal actions (set to -inf)
  auto masked_policy = torch::where(
      legal_mask, policy,
      torch::full_like(policy, -std::numeric_limits<f64>::infinity()));

  // Apply softmax to get probabilities
  torch::Tensor probs = torch::softmax(masked_policy, 0, torch::kFloat64);

  // Add children nodes with proper probabilities
  for (ActionId action : legal_actions) {
    auto p = probs[action.value()].template item<f64>();
    NodeId child = nodes_.create(action, p);
    nodes_.get(node_id).children.push_back(child);
  }

  Node& node = nodes_.get(node_id);
  node.played_by = player;

  return value.template item<f64>();
}

}  // namespace DamathZero::Core
