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
  constexpr auto next() const -> Player { return Player(not is_first); }

  constexpr auto operator==(const Player& player) const -> bool = default;

  static const Player First;
  static const Player Second;

 private:
  constexpr Player(bool is_first) : is_first(is_first) {}
  constexpr Player() {}

 private:
  bool is_first = false;
};

inline const Player Player::First = Player(true);
inline const Player Player::Second = Player(false);

struct ActionId : Base::Id {
  using Id::Id;
};

template <typename G>
concept Game = requires(G g, ActionId id) {
  { g.get_action_size() } -> std::same_as<u64>;

  { g.is_terminal() } -> std::same_as<bool>;
  { g.apply(id) } -> std::same_as<void>;

  { g.make_image() } -> std::same_as<torch::Tensor>;
  { g.make_target() } -> std::same_as<torch::Tensor>;

  { g.get_history() } -> std::same_as<std::span<ActionId>>;

  { g.get_current_player() } -> std::same_as<Player>;
  { g.get_legal_actions() } -> std::same_as<std::vector<ActionId>>;
};

template <Game Game>
class ReplayBuffer {
 public:
  ReplayBuffer(Config config) : config_(config) {
    games_.reserve(config.buffer_size);
  };

  auto save_game(Game game) -> void;
  auto sample_batch() const -> torch::Tensor;

 private:
  Config config_;
  std::vector<Game> games_;
};

template <Game Game>
auto ReplayBuffer<Game>::save_game(Game game) -> void {
  games_.push_back(game);
};

template <Game Game>
auto ReplayBuffer<Game>::sample_batch() const -> torch::Tensor {
  // Calculate history sizes for each game
  std::vector<i64> history_sizes;
  history_sizes.reserve(games_.size());

  for (const auto& game : games_) {
    history_sizes.push_back(static_cast<i64>(game.get_history().size()));
  }

  // Convert to tensor
  auto history_sizes_tensor = torch::tensor(history_sizes, torch::kInt64);

  // Create probability distribution (normalize by sum)
  auto probabilities = history_sizes_tensor.to(torch::kFloat64) /
                       history_sizes_tensor.sum(0).to(torch::kFloat64);

  // Sample game indices according to this distribution
  auto sampled_indices = torch::multinomial(probabilities, config_.batch_size,
                                            /*replacement=*/false);

  std::vector<Game> random_games;
  random_games.reserve(config_.batch_size);

  for (auto i = 0; i < config_.batch_size; ++i) {
    auto idx = sampled_indices[i].template item<i64>();
    random_games.push_back(games_[idx]);
  }

  return torch::tensor({0, 0});
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_GAME_H
