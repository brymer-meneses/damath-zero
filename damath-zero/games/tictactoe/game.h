#ifndef DAMATH_ZERO_GAMES_TICTACTOE_H
#define DAMATH_ZERO_GAMES_TICTACTOE_H

#include <torch/torch.h>

#include <span>
#include <vector>

#include "damath-zero/base/macros.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/game.h"

namespace DamathZero::Games::TicTacToe {

class Game {
 public:
  auto is_terminal() -> bool;
  auto apply(Core::ActionId id) -> void;

  auto make_image(Core::StateIndex id) const -> torch::Tensor;
  auto make_target(Core::StateIndex id) const -> torch::Tensor;

  auto get_terminal_value() const -> f64;

  auto get_legal_actions() const -> std::vector<Core::ActionId>;

  constexpr auto get_action_size() const -> u64 { return 9; }
  constexpr auto get_history() const -> std::span<const Core::ActionId> {
    return history_;
  }
  constexpr auto get_current_player() const -> Core::Player { return player_; }

 private:
  std::vector<Core::ActionId> history_;
  std::array<i8, 9> board_ = {0};
  Core::Player player_ = Core::Player::First;
};

REQUIRE_CONCEPT(Core::Game, Game);

}  // namespace DamathZero::Games::TicTacToe

#endif  // !DAMATH_ZERO_GAMES_TICTACTOE_H
