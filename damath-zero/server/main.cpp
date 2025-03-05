#include <glaze/json/write.hpp>
#include <print>

#include "damath-zero/core/board.h"
#include "damath-zero/games/tictactoe/board.h"
#include "damath-zero/server/tictactoe.h"

auto main(int argc, char** argv) -> int {
  Response response{
      .id = GameId(1),
      .board = TicTacToe::Board(),
      .player = Core::Player::First,
      .result = Core::GameResult::Invalid,
  };
  std::println("{}", glz::write_json(response).value_or("error"));
  /*GameServer{}.start();*/

  return 0;
}
