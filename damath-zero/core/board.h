#ifndef DAMATH_ZERO_CORE_BOARD_H
#define DAMATH_ZERO_CORE_BOARD_H

#include <torch/torch.h>

#include <concepts>

#include "damath-zero/base/id.h"

namespace DamathZero::Core {

struct ActionId : Base::Id {
  using Id::Id;

  static const ActionId Invalid;
};

inline const ActionId ActionId::Invalid = ActionId(-1);

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

// A Board represents the current state or snapshot of the game.
// Meanwhile a Game serves as an object that contains the history of the
// moves played in the board.
//
// Ideally, any object that implements this interface should be relatively cheap
// to copy. Since it only holds the rules and the current state of the positions
// of the elements in the board.
template <typename B>
concept Board = requires(B b, ActionId id, Player player) {
  { b.is_terminal(player) } -> std::same_as<bool>;
  { b.apply(player, id) } -> std::same_as<std::pair<Player, B>>;

  { b.get_feature(player) } -> std::same_as<torch::Tensor>;
  { b.get_legal_actions(player) } -> std::same_as<std::vector<ActionId>>;

  std::is_same_v<decltype(B::ActionSize), u64>;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_BOARD_H
