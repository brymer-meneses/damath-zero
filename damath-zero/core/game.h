#ifndef DAMATH_ZERO_CORE_GAME_H
#define DAMATH_ZERO_CORE_GAME_H

#include <torch/torch.h>

#include <glaze/glaze.hpp>
#include <vector>

#include "damath-zero/base/id.h"
#include "damath-zero/core/board.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/node.h"

namespace DamathZero::Core {

struct StateIndex : Base::Id {
  using Id::Id;

  constexpr auto is_last() const -> bool { return Id::value() == -1; }

  static const StateIndex Last;
};

inline const StateIndex StateIndex::Last = StateIndex(-1);

template <Board Board>
class Game {
 public:
  constexpr Game() { history_.push_back({Player::First, Board()}); }

  auto get_feature(StateIndex state_index) const -> torch::Tensor;
  auto get_label(StateIndex state_index) const -> torch::Tensor;

  auto store_search_statistics(const NodeStorage& nodes, NodeId root_id)
      -> void;

  auto apply(ActionId id) -> void;

  constexpr auto history_size() const -> u64 { return history_.size(); }

  constexpr auto current_player() const -> Player { return history_.back()[0]; }
  constexpr auto current_board() const -> const Board& {
    return history_.back()[1];
  }

  constexpr auto is_terminal() const -> bool {
    return current_board().is_terminal();
  }

 private:
  std::vector<std::pair<Player, Board>> history_;
  std::vector<torch::Tensor> child_visits_;
};

template <Board Board>
auto Game<Board>::apply(ActionId action_id) -> void {
  history_.push_back(current_board().apply(current_player(), action_id));
}

template <Board Board>
auto Game<Board>::get_feature(StateIndex state_index) const -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  const auto [player, board] = history_[index];
  return board.get_feature(player);
}

template <Board Board>
auto Game<Board>::get_label(StateIndex state_index) const -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  return child_visits_[index];
}

template <Board Board>
auto Game<Board>::store_search_statistics(const NodeStorage& nodes,
                                          NodeId root_id) -> void {
  auto root = nodes.get(root_id);

  auto sum_visits = std::ranges::fold_left(
      root.children | std::views::transform([&nodes](NodeId child_id) {
        return nodes.get(child_id).visit_count;
      }),
      0, std::plus<u64>());

  auto visits = torch::zeros({Board::ActionSize}, torch::kFloat64);
  for (const auto child_id : root.children) {
    const auto& child = nodes.get(child_id);
    visits[child.action_taken.value()] = child.visit_count / sum_visits;
  }

  child_visits_.push_back(visits);
}

struct PredictionPair {
  torch::Tensor data;
  torch::Tensor target;

  PredictionPair(torch::Tensor data, torch::Tensor target)
      : data(data), target(target) {};
};

template <Board Board>
class ReplayBuffer {
  using Game = Game<Board>;

 public:
  constexpr ReplayBuffer(Config config) : config_(config) {
    games_.reserve(config.buffer_size);
  };

  auto save_game(Game game) -> void;
  auto sample_batch() const -> std::vector<PredictionPair>;

 private:
  Config config_;
  std::vector<Game> games_;
};

template <Board Board>
auto ReplayBuffer<Board>::save_game(Game game) -> void {
  games_.push_back(game);
};

template <Board Board>
auto ReplayBuffer<Board>::sample_batch() const -> std::vector<PredictionPair> {
  auto history_sizes = games_ | std::views::transform([](const auto& game) {
                         return game.get_history().size();
                       }) |
                       std::ranges::to<std::vector<i64>>();

  auto history_sizes_tensor = torch::tensor(history_sizes, torch::kInt64);
  auto probabilities = history_sizes_tensor.to(torch::kFloat64) /
                       history_sizes_tensor.sum(0).to(torch::kFloat64);
  auto sampled_indices = torch::multinomial(probabilities, config_.batch_size,
                                            /*replacement=*/false);

  std::vector<PredictionPair> batch;
  batch.reserve(config_.batch_size);

  for (auto i = 0; i < config_.batch_size; i++) {
    auto game_index = sampled_indices[i].template item<i64>();
    auto& game = games_[game_index];
    auto random_position = StateIndex(std::rand() % game.get_history().size());

    batch.emplace_back(game.make_feature(random_position),
                       game.make_target(random_position));
  }

  return batch;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_GAME_H
