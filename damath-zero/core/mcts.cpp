#include "damath-zero/core/mcts.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <ranges>

using namespace DamathZero::Core;

auto MCTS::backpropagate(std::span<NodeId> path, f64 value, Player player)
    -> void {
  for (const NodeId node_id : path) {
    auto& node = nodes_.get(node_id);

    node.visit_count += 1;

    if (node.played_by == player) {
      node.value_sum += (1 - value);
    } else {
      node.value_sum += value;
    }
  }
}

auto MCTS::compute_puct_score(NodeId parent_id, NodeId child_id) const -> f64 {
  const auto& parent = nodes_.get(parent_id);
  const auto& child = nodes_.get(child_id);

  auto pb_c =
      std::log((parent.visit_count + config_.c_base + 1.0) / config_.c_base) +
      config_.c_init;
  pb_c *= std::sqrt(parent.visit_count / (child.visit_count + 1.0));

  const auto prior_score = pb_c * child.prior;
  const auto value_score = child.mean_action_value();
  return prior_score + value_score;
}

auto MCTS::select_highest_puct_score(NodeId parent_id) const -> NodeId {
  const auto& parent = nodes_.get(parent_id);
  assert(parent.is_expanded());

  return *std::ranges::max_element(parent.children,
                                   [this, parent_id](NodeId n1, NodeId n2) {
                                     return compute_puct_score(parent_id, n1) <
                                            compute_puct_score(parent_id, n2);
                                   });
}

auto MCTS::select_highest_visit_count(NodeId parent_id,
                                      i32 game_history_size) const -> NodeId {
  const auto& parent = nodes_.get(parent_id);
  assert(parent.is_expanded());

  if (game_history_size < config_.num_sampling_moves) {
    auto visit_counts = parent.children |
                        std::views::transform([this](NodeId n) {
                          return static_cast<f64>(nodes_.get(n).visit_count);
                        }) |
                        std::ranges::to<std::vector<f64>>();
    auto probs = torch::softmax(torch::tensor(visit_counts, torch::kF64), 0);
    auto child_index = torch::multinomial(probs, 1).item<i32>();
    return parent.children[child_index];
  }

  return *std::ranges::max_element(
      parent.children, [this](NodeId n1, NodeId n2) {
        return nodes_.get(n1).visit_count < nodes_.get(n2).visit_count;
      });
}

std::random_device rd;
std::mt19937 gen(rd());

auto MCTS::add_exploration_noise(NodeId node_id) -> void {
  std::gamma_distribution<> noise(config_.root_dirichlet_alpha, 1);
  auto frac = config_.root_exploration_fraction;

  for (NodeId child_id : nodes_.get(node_id).children) {
    auto& child_node = nodes_.get(child_id);
    auto generated_noise = noise(gen);
    child_node.prior = child_node.prior * (1 - frac) + frac * generated_noise;
  }
}
