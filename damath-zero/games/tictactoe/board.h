#pragma once

#include <torch/torch.h>

#include <vector>

#include "damath-zero/base/macros.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/board.h"

namespace DamathZero::Games::TicTacToe {

struct Board {
  constexpr explicit Board(std::array<i8, 9> data) : data(data) {}
  constexpr Board() = default;

  auto is_terminal(Core::Player) const -> bool;
  auto apply(Core::Player, Core::ActionId) -> std::pair<Core::Player, Board>;

  auto get_result(Core::Player player) const -> Core::GameResult;
  auto get_feature(Core::Player) const -> torch::Tensor;
  auto get_legal_actions(Core::Player) const -> std::vector<Core::ActionId>;

  static constexpr auto ActionSize = {9};
  static constexpr auto FeatureShape = {9};

  std::array<i8, 9> data = {0};
};

REQUIRE_CONCEPT(Core::Board, Board);

}  // namespace DamathZero::Games::TicTacToe
//
template <>
struct glz::meta<DamathZero::Games::TicTacToe::Board> {
  using Board = DamathZero::Games::TicTacToe::Board;

  static constexpr auto value{&Board::data};
};
