#pragma once

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

struct Target {
  torch::Tensor policy;
  torch::Tensor value;
};

template <Concepts::Board Board>
class Game {
 public:
  auto get_feature(StateIndex state_index) const -> torch::Tensor;
  auto get_child_visits(StateIndex state_index) const -> torch::Tensor;
  auto get_value(StateIndex state_index) const -> torch::Tensor;

  auto store_search_statistics(const NodeStorage& nodes, NodeId root_id)
      -> void;

  auto apply(ActionId id) -> void;

  constexpr auto history_size() const -> u64 { return history_.size(); }

  constexpr auto to_play() const -> Player { return history_.back().to_play; }
  constexpr auto board() const -> const Board& { return history_.back().board; }

  constexpr auto is_terminal() const -> bool {
    auto [to_play, board] = history_.back();
    return board.is_terminal(to_play);
  }

 private:
  struct State {
    Player to_play;
    Board board;

    constexpr State(Player player, Board board)
        : to_play(player), board(board) {}

    constexpr State(std::pair<Player, Board> data)
        : to_play(data.first), board(data.second) {}
  };

  std::vector<State> history_ = {{Player::First, Board()}};
  std::vector<torch::Tensor> child_visits_;
};

template <Concepts::Board Board>
auto Game<Board>::apply(ActionId action_id) -> void {
  auto [to_play, board] = history_.back();
  history_.push_back(board.apply(to_play, action_id));
}

template <Concepts::Board Board>
auto Game<Board>::get_value(StateIndex state_index) const -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  auto to_play = history_[index].to_play;
  auto board = history_.back().board;

  f64 value;
  switch (board.get_result(to_play)) {
    case GameResult::Win:
      value = 1;
      break;
    case GameResult::Loss:
      value = -1;
      break;
    case GameResult::Draw:
      value = 0;
      break;
    case GameResult::Invalid:
      value = -2;
      break;
  }

  return torch::tensor({value});
}

template <Concepts::Board Board>
auto Game<Board>::get_feature(StateIndex state_index) const -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  const auto [to_play, board] = history_[index];
  return board.get_feature(to_play);
}

template <Concepts::Board Board>
auto Game<Board>::get_child_visits(StateIndex state_index) const
    -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  return child_visits_[index];
}

template <Concepts::Board Board>
auto Game<Board>::store_search_statistics(const NodeStorage& nodes,
                                          NodeId root_id) -> void {
  auto& root = nodes.get(root_id);

  auto sum_visits = std::ranges::fold_left(
      root.children | std::views::transform([&nodes](NodeId child_id) {
        return static_cast<f64>(nodes.get(child_id).visit_count);
      }),
      0.0, std::plus<f64>());

  auto visits = torch::zeros({9}, torch::kFloat64);
  for (const auto child_id : root.children) {
    const auto& child = nodes.get(child_id);
    visits[child.action_taken.value()] =
        static_cast<f64>(child.visit_count) / sum_visits;
  }

  child_visits_.push_back(visits);
}

struct Batch {
  torch::Tensor input_features;
  torch::Tensor target_values;
  torch::Tensor target_policies;
};

template <Concepts::Board Board>
class ReplayBuffer {
  using Game = Game<Board>;

 public:
  constexpr ReplayBuffer(Config config) : config_(config) {
    games_.reserve(config.buffer_size);
  };

  auto save_game(Game game) -> void;
  auto sample_batch() const -> Batch;

  constexpr auto size() const -> u64 { return games_.size(); };

 private:
  mutable std::mutex mutex_;
  Config config_;
  std::vector<Game> games_;
};

template <Concepts::Board Board>
auto ReplayBuffer<Board>::save_game(Game game) -> void {
  mutex_.lock();
  games_.push_back(std::move(game));
  mutex_.unlock();
};

template <Concepts::Board Board>
auto ReplayBuffer<Board>::sample_batch() const -> Batch {
  mutex_.lock();
  auto history_sizes = games_ | std::views::transform([](const auto& game) {
                         return game.history_size();
                       }) |
                       std::ranges::to<std::vector<i64>>();
  mutex_.unlock();

  auto history_sizes_tensor = torch::tensor(history_sizes, torch::kInt64);
  auto probabilities = history_sizes_tensor.to(torch::kFloat64) /
                       history_sizes_tensor.sum(0).to(torch::kFloat64);
  auto sampled_indices = torch::multinomial(probabilities, config_.batch_size,
                                            /*replacement=*/false);

  std::vector<torch::Tensor> features;
  std::vector<torch::Tensor> values;
  std::vector<torch::Tensor> policies;

  features.reserve(config_.batch_size);
  values.reserve(config_.batch_size);
  policies.reserve(config_.batch_size);

  for (auto i = 0; i < config_.batch_size; i++) {
    auto game_index = sampled_indices[i].template item<i64>();
    auto& game = games_[game_index];
    auto random_position =
        StateIndex(torch::randint(0, game.history_size() - 1, {1})
                       .template item<i32>());  // TODO: config.tau0 for a

    features.emplace_back(game.get_feature(random_position));
    values.emplace_back(game.get_value(random_position));
    policies.emplace_back(game.get_child_visits(random_position));
  }

  return {
      .input_features = torch::stack(features, 0),
      .target_values = torch::stack(values, 0),
      .target_policies = torch::stack(policies, 0),
  };
};

}  // namespace DamathZero::Core
