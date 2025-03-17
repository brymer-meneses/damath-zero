#pragma once

#include <torch/torch.h>

#include <vector>

#include "damath-zero/base/macros.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/board.h"

namespace DamathZero::Games::Connect2 {

struct Board {
  constexpr explicit Board(std::vector<i8> data) : data(data) {}
  constexpr Board() = default;

  auto is_terminal(Core::Player) const -> bool;
  auto apply(Core::Player, Core::ActionId) -> std::pair<Core::Player, Board>;

  auto get_result(Core::Player player) const -> Core::GameResult;
  auto get_feature(Core::Player) const -> torch::Tensor;
  auto get_legal_actions(Core::Player) const -> std::vector<Core::ActionId>;

  static constexpr auto ActionSize = 4;
  static constexpr auto FeatureShape = 4;

  std::vector<i8> data = std::vector<i8>(FeatureShape, 0);
};

REQUIRE_CONCEPT(Core::Concepts::Board, Board);

}  // namespace DamathZero::Games::Connect2
//
template <>
struct glz::meta<DamathZero::Games::Connect2::Board> {
  using Board = DamathZero::Games::Connect2::Board;

  static constexpr auto value{&Board::data};
};
