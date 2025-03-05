#pragma once

#include <httplib.h>

#include <glaze/ext/jsonrpc.hpp>

#include "damath-zero/base/storage.h"
#include "damath-zero/base/types.h"
#include "damath-zero/games/tictactoe/board.h"

using namespace DamathZero;
using namespace DamathZero::Games;

struct GameId : Base::Id {
  using Id::Id;
};

struct Game {
  TicTacToe::Board board = TicTacToe::Board{};
  Core::Player player = Core::Player::First;
};

struct GameStorage : Base::Storage<GameId, Game> {
  using Storage::Storage;
};

struct New {
  struct Request {};
};

struct Get {
  struct Request {
    i32 id;
  };
};

struct Move {
  struct Request {
    i32 id;
    i32 cell;
  };
};

struct Response {
  i32 id;
  std::array<i8, 9> board;
  i8 player;
  i8 result;
};

class GameServer {
 public:
  auto listen(std::string_view hostname = "localhost", u16 port = 8080) -> void;

  GameServer();

 private:
  httplib::Server http_server_;
  glz::rpc::server<glz::rpc::method<"new", New::Request, Response>,
                   glz::rpc::method<"get", Get::Request, Response>,
                   glz::rpc::method<"move", Move::Request, Response>>
      rpc_server_;

  GameStorage games_;
};