#include <damath-zero/core/mcts.h>

#include <algorithm>
#include <cmath>

using namespace DamathZero::Core;

auto MCTS::backpropagate(std::span<NodeId> path, f64 value, Player player)
    -> void {
  for (const NodeId node_id : path) {
    auto& node = nodes_.get(node_id);

    node.visit_count += 1;

    if (node.played_by == player) {
      node.value_sum += value;
    } else {
      node.value_sum += 1 - value;
    }
  }
}

auto MCTS::compute_puct_score(NodeId parent_id, NodeId child_id) const -> f64 {
  const auto& parent = nodes_.get(parent_id);
  const auto& child = nodes_.get(child_id);

  auto pb_c =
      std::log((parent.visit_count + config_.c_base + 1) / config_.c_base);
  pb_c *= std::sqrt(parent.visit_count / (child.visit_count + 1));

  const auto prior_score = pb_c * child.prior;
  const auto value_score = child.mean_action_value();
  return prior_score + value_score;
}

auto MCTS::select_highest_puct_score(NodeId parent_id) const -> NodeId {
  const auto& parent = nodes_.get(parent_id);
  assert(parent.is_expanded());

  return *std::max_element(parent.children.begin(), parent.children.end(),
                           [this, parent_id](const NodeId n1, const NodeId n2) {
                             return compute_puct_score(parent_id, n1) <
                                    compute_puct_score(parent_id, n2);
                           });
}

auto MCTS::select_highest_visit_count(NodeId parent_id) const -> NodeId {
  const auto& parent = nodes_.get(parent_id);
  assert(parent.is_expanded());

  // TODO: do something different if game.history is not sufficient
  return *std::max_element(parent.children.begin(), parent.children.end(),
                           [this](const NodeId n1, const NodeId n2) {
                             return nodes_.get(n1).visit_count <
                                    nodes_.get(n2).visit_count;
                           });
}
