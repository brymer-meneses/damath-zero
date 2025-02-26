#ifndef DAMATH_ZERO_CORE_MCTS_H
#define DAMATH_ZERO_CORE_MCTS_H

#include <torch/torch.h>

#include <cassert>
#include <span>

#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/network.h"
#include "damath-zero/core/node.h"

namespace DamathZero::Core {

class MCTS {
 public:
  MCTS(Config config) : config_(config) {}

  auto run(Game auto game, Network auto network) -> NodeId;

  auto nodes() const -> NodeStorage const&;
  auto nodes() -> NodeStorage&;

 private:
  auto select_highest_puct_score(NodeId parent) const -> NodeId;
  auto select_highest_visit_count(NodeId parent) const -> NodeId;
  auto compute_puct_score(NodeId parent, NodeId child) const -> f64;

  auto backpropagate(std::span<NodeId> path, f64 value, Player player) -> void;

  auto expand_node(NodeId node, Game auto game, Network auto network) -> f64;

 private:
  NodeStorage nodes_;
  Config config_;
};

auto MCTS::run(Game auto game, Network auto network) -> NodeId {
  c10::InferenceMode guard;

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

auto MCTS::expand_node(NodeId node_id, Game auto game, Network auto network)
    -> f64 {
  auto result = network.inference(game.make_image());
  auto value = result[0];
  auto policy = result[1];

  auto& node = nodes_.get(node_id);
  node.played_by = game.get_current_player();
  auto legal_actions = game.get_legal_actions();

  // Create a boolean mask for legal actions
  auto legal_mask = torch::zeros_like(policy, torch::kBool);
  for (ActionId action : legal_actions) {
    legal_mask[action.value()] = true;
  }

  // Use where to mask illegal actions (set to -inf)
  auto masked_policy = torch::where(
      legal_mask, policy,
      torch::full_like(policy, -std::numeric_limits<float>::infinity()));

  // Apply softmax to get probabilities
  torch::Tensor probs = torch::softmax(masked_policy, 0);

  // Add children nodes with proper probabilities
  for (ActionId action : legal_actions) {
    auto p = probs[action.value()].item<f64>();
    node.children.push_back(nodes_.create(action, p));
  }

  return value;
}

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_MCTS_H
