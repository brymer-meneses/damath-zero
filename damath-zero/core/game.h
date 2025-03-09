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
  f64 value;
};

template <Board Board>
class Game {
 public:
  auto get_feature(StateIndex state_index) const -> torch::Tensor;

  auto get_target(StateIndex state_index) const -> Target;

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

template <Board Board>
auto Game<Board>::apply(ActionId action_id) -> void {
  auto [to_play, board] = history_.back();
  history_.push_back(board.apply(to_play, action_id));
}

template <Board Board>
auto Game<Board>::get_feature(StateIndex state_index) const -> torch::Tensor {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  const auto [to_play, board] = history_[index];
  return board.get_feature(to_play);
}

template <Board Board>
auto Game<Board>::get_target(StateIndex state_index) const -> Target {
  auto index =
      state_index.is_last() ? history_.size() - 1 : state_index.value();
  auto [to_play, board] = history_[index];

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
  }

  return {child_visits_[index], value};
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
  torch::Tensor feature;
  Target target;

  PredictionPair(torch::Tensor feature, Target target)
      : feature(feature), target(target) {};
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
  mutable std::mutex mutex_;
  Config config_;
  std::vector<Game> games_;
};

template <Board Board>
auto ReplayBuffer<Board>::save_game(Game game) -> void {
  mutex_.lock();
  games_.push_back(std::move(game));
  mutex_.unlock();
};

template <Board Board>
auto ReplayBuffer<Board>::sample_batch() const -> std::vector<PredictionPair> {
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

  std::vector<PredictionPair> batch;
  batch.reserve(config_.batch_size);

  for (auto i = 0; i < config_.batch_size; i++) {
    auto game_index = sampled_indices[i].template item<i64>();
    auto& game = games_[game_index];
    auto random_position = StateIndex(std::rand() % game.history_size());

    batch.emplace_back(game.get_feature(random_position),
                       game.get_target(random_position));
  }

  return batch;
};

}  // namespace DamathZero::Core
