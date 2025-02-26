#ifndef DAMATH_ZERO_CORE_GAME_H
#define DAMATH_ZERO_CORE_GAME_H

#include <torch/torch.h>

#include <concepts>
#include <span>

#include "damath-zero/base/id.h"

namespace DamathZero::Core {

class Player {
 public:
  static constexpr auto first() -> Player { return Player(true); }
  static constexpr auto second() -> Player { return Player(false); }

  constexpr auto next() const -> Player { return Player(not is_first); }

  constexpr auto operator==(Player player) -> bool {
    return player.is_first == is_first;
  }

 private:
  constexpr Player(bool is_first) : is_first(is_first) {}
  constexpr Player() {}

 private:
  bool is_first = false;
};

struct ActionId : Base::Id {
  using Id::Id;
};

template <typename G>
concept Game = requires(G g, ActionId id) {
  { G::action_size() } -> std::same_as<i64>;

  { g.is_terminal() } -> std::same_as<bool>;
  { g.clone() } -> std::same_as<G>;
  { g.apply(id) } -> std::same_as<void>;

  { g.make_image() } -> std::same_as<torch::Tensor>;

  { g.get_history() } -> std::same_as<std::span<ActionId>>;

  { g.get_current_player() } -> std::same_as<Player>;
  { g.get_legal_actions() } -> std::same_as<std::vector<ActionId>>;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_GAME_H
