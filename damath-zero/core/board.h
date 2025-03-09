#pragma once

#include <torch/torch.h>

#include <glaze/glaze.hpp>
#include <string_view>

#include "damath-zero/base/id.h"

namespace DamathZero::Core {

struct ActionId : Base::Id {
  using Id::Id;

  static const ActionId Invalid;
};

inline const ActionId ActionId::Invalid = ActionId(-1);

class Player {
 public:
  enum Value : i8 {
    First = 1,
    Second = -1,
    Invalid = 0,
  };
  constexpr Player(Value value) : value_(value) {}

  constexpr auto operator==(Player player) const -> bool {
    return player.value_ == value_;
  }

  constexpr auto next() const -> Player {
    return value_ == Value::First ? Player::Second : Player::First;
  }

  constexpr auto value() const -> i8 { return value_; }

 public:
 private:
  constexpr Player() = default;

 private:
  Value value_;
};

enum class GameResult : i8 {
  Win = 1,
  Draw = 0,
  Loss = -1,
  Invalid = -2,
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
  { B() } -> std::same_as<B>;

  { b.get_result(player) } -> std::same_as<GameResult>;

  { b.is_terminal(player) } -> std::same_as<bool>;
  { b.apply(player, id) } -> std::same_as<std::pair<Player, B>>;

  { b.get_feature(player) } -> std::same_as<torch::Tensor>;
  { b.get_legal_actions(player) } -> std::same_as<std::vector<ActionId>>;

  std::is_same_v<decltype(B::ActionSize), u64>;
};

}  // namespace DamathZero::Core

template <>
struct glz::meta<DamathZero::Core::Player> {
  using Player = DamathZero::Core::Player;
  static constexpr auto value = [](auto& p) -> std::string_view {
    if (p == Player::First) {
      return "first";
    } else if (p == Player::Second) {
      return "second";
    } else {
      return "invalid";
    }
  };
};
