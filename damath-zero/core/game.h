#ifndef DAMATH_ZERO_CORE_GAME_H
#define DAMATH_ZERO_CORE_GAME_H

#include <torch/torch.h>

#include <concepts>
#include <glaze/glaze.hpp>
#include <span>
#include <vector>

#include "damath-zero/base/id.h"
#include "damath-zero/core/config.h"

namespace DamathZero::Core {

class Player {
 public:
  enum Value {
    First,
    Second,
  };
  constexpr Player(Value value) : value_(value) {}

  constexpr auto operator==(Player player) -> bool {
    return player.value_ == value_;
  }

  constexpr auto next() const -> Player {
    if (value_ == Value::First) {
      return Player::Second;
    }
    return Player::First;
  }

 private:
  constexpr Player() {}

 private:
  Value value_;
};

struct ActionId : Base::Id {
  using Id::Id;

  static const ActionId Invalid;
};

inline const ActionId ActionId::Invalid = ActionId(-1);

struct StateIndex : Base::Id {
  using Id::Id;

  static const StateIndex Last;
};

inline const StateIndex StateIndex::Last = StateIndex(-1);

template <typename G>
concept Game = requires(G g, ActionId id, StateIndex state_id) {
  { g.is_terminal() } -> std::same_as<bool>;
  { g.apply(id) } -> std::same_as<void>;

  { g.make_image(state_id) } -> std::same_as<torch::Tensor>;
  { g.make_target(state_id) } -> std::same_as<torch::Tensor>;

  { g.get_terminal_value() } -> std::same_as<f64>;

  { g.get_action_size() } -> std::same_as<u64>;
  { g.get_history() } -> std::same_as<std::span<const ActionId>>;
  { g.get_current_player() } -> std::same_as<Player>;
  { g.get_legal_actions() } -> std::same_as<std::vector<ActionId>>;
};

struct PredictionPair {
  torch::Tensor data;
  torch::Tensor target;

  PredictionPair(torch::Tensor data, torch::Tensor target)
      : data(data), target(target) {};
};

template <Game Game>
class ReplayBuffer {
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

template <Game Game>
auto ReplayBuffer<Game>::save_game(Game game) -> void {
  games_.push_back(game);
};

template <Game Game>
auto ReplayBuffer<Game>::sample_batch() const -> std::vector<PredictionPair> {
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

    batch.emplace_back(game.make_image(random_position),
                       game.make_target(random_position));
  }

  return batch;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_GAME_H
