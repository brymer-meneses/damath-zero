#include "damath-zero/games/tictactoe/game.h"

#include <array>

using namespace DamathZero::Games::TicTacToe;

auto Game::is_terminal() -> bool {
  if (std::all_of(board_.begin(), board_.end(), [](auto x) { return x != 0; }))
    return true;

  const std::array<std::array<int, 3>, 8> win_conditions{{
      {0, 1, 2},
      {3, 4, 5},
      {6, 7, 8},
      {0, 3, 6},
      {1, 4, 7},
      {2, 5, 8},
      {0, 4, 8},
      {2, 4, 6},
  }};

  if (std::any_of(win_conditions.begin(), win_conditions.end(),
                  [&](auto board) {
                    return board[0] == board[1] and board[1] == board[2] and
                           board[0] != 0;
                  }))
    return true;

  return false;
}

auto Game::apply(Core::ActionId id) -> void {
  if (player_ == Core::Player::First) {
    board_[id.value()] = 1;
  } else {
    board_[id.value()] = -1;
  }

  player_ = player_.next();
}

auto Game::make_image(Core::StateIndex id) const -> torch::Tensor {
  return torch::tensor(board_.data());
}

auto Game::make_target(Core::StateIndex id) const -> torch::Tensor {
  return torch::tensor(board_.data());
}

auto Game::get_legal_actions() const -> std::vector<Core::ActionId> {
  std::vector<Core::ActionId> legal_actions;

  for (auto i = 0; i < board_.size(); i++) {
    if (board_[i] == 0)
      legal_actions.push_back(Core::ActionId(i));
  }

  return legal_actions;
}
