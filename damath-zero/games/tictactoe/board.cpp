#include "damath-zero/games/tictactoe/board.h"

#include <array>

#include "damath-zero/core/board.h"

using namespace DamathZero::Games::TicTacToe;

static constexpr std::array<std::array<int, 3>, 8> win_conditions{{
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {2, 4, 6},
}};

auto Board::get_result(Core::Player player) const -> Core::GameResult {
  if (not is_terminal(player))
    return Core::GameResult::Invalid;

  auto did_win = [this](auto player) {
    return std::ranges::any_of(win_conditions, [&](auto board) {
      return board[0] == board[1] and board[1] == board[2] and
             board[0] == player.value();
    });
  };

  if (did_win(player))
    return Core::GameResult::Win;
  else if (did_win(player.next()))
    return Core::GameResult::Loss;
  else if (std::ranges::all_of(board_, [](auto x) { return x != 0; }))
    return Core::GameResult::Draw;
}

auto Board::is_terminal(Core::Player) const -> bool {
  if (std::all_of(board_.begin(), board_.end(), [](auto x) { return x != 0; }))
    return true;

  if (std::any_of(win_conditions.begin(), win_conditions.end(),
                  [&](auto board) {
                    return board[0] == board[1] and board[1] == board[2] and
                           board[0] != 0;
                  }))
    return true;

  return false;
}

auto Board::apply(Core::Player player, Core::ActionId id)
    -> std::pair<Core::Player, Board> {
  board_[id.value()] = player.value();
  return {player.next(), Board(board_)};
}

auto Board::get_feature(Core::Player) const -> torch::Tensor {
  return torch::tensor(board_.data());
}

auto Board::get_legal_actions(Core::Player) const
    -> std::vector<Core::ActionId> {
  std::vector<Core::ActionId> legal_actions;

  for (auto i = 0; i < board_.size(); i++) {
    if (board_[i] == 0)
      legal_actions.push_back(Core::ActionId(i));
  }

  return legal_actions;
}
