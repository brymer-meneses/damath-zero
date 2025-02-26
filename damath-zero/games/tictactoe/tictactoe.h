#ifndef DAMATH_ZERO_GAMES_TICTACTOE_H
#define DAMATH_ZERO_GAMES_TICTACTOE_H

#include <torch/torch.h>

#include <span>
#include <vector>

#include "damath-zero/core/game.h"

namespace DamathZero::Games {

class TicTacToe {
 public:
  auto is_terminal() -> bool;
  auto clone() -> TicTacToe;
  auto apply(Core::ActionId id) -> void;

  auto make_image() -> torch::Tensor;
  auto make_target() -> torch::Tensor;

  auto get_legal_actions() -> std::span<Core::ActionId>;

  constexpr auto get_action_size() const -> u64 { return 9; }
  constexpr auto get_history() -> std::span<Core::ActionId> { return history_; }
  constexpr auto get_current_player() const -> Core::Player { return player_; }

 private:
  std::vector<Core::ActionId> history_;
  Core::Player player_;
};

static_assert(Core::Game<TicTacToe>,
              "TicTacToe must conform to the Core::Game interface.");

}  // namespace DamathZero::Games

#endif  // !DAMATH_ZERO_GAMES_TICTACTOE_H
