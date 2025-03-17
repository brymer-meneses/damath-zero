#include "damath-zero/games/connect2/board.h"

#include <array>

#include "damath-zero/core/board.h"

using namespace DamathZero::Games::Connect2;
using namespace DamathZero;

static constexpr std::array<std::array<int, 2>, 3> win_conditions{
    {{0, 1}, {1, 2}, {2, 3}}};

auto Board::get_result(Core::Player player) const -> Core::GameResult {
  auto did_win = [this](auto player) {
    for (const auto& [x, y] : win_conditions) {
      if (data[x] == data[y] and data[x] == player.value())
        return true;
    }

    return false;
  };

  if (did_win(player))
    return Core::GameResult::Win;
  else if (did_win(player.next()))
    return Core::GameResult::Loss;
  else if (std::ranges::none_of(data, [](auto x) { return x == 0; }))
    return Core::GameResult::Draw;
  else
    return Core::GameResult::Invalid;
}

auto Board::is_terminal(Core::Player) const -> bool {
  if (std::ranges::none_of(data, [](auto x) { return x == 0; }))
    return true;

  for (const auto& [x, y] : win_conditions) {
    if (data[x] == data[y] and data[x] != 0)
      return true;
  }

  return false;
}

auto Board::apply(Core::Player player, Core::ActionId id)
    -> std::pair<Core::Player, Board> {
  data[id.value()] = player.value();
  return {player.next(), Board(data)};
}

auto Board::get_feature(Core::Player player) const -> torch::Tensor {
  if (player == Core::Player::Second)
    return -torch::tensor(data, torch::kFloat32);

  return torch::tensor(data, torch::kFloat32);
}

auto Board::get_legal_actions(Core::Player) const
    -> std::vector<Core::ActionId> {
  std::vector<Core::ActionId> legal_actions = {};

  for (auto i = 0; i < ActionSize; i++) {
    if (data[i] == 0) {
      legal_actions.push_back(Core::ActionId(i));
    }
  }

  return legal_actions;
}
