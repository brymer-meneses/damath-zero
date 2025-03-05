#pragma once

#include <ixwebsocket/IXWebSocketServer.h>

#include <glaze/ext/jsonrpc.hpp>

#include "damath-zero/base/storage.h"
#include "damath-zero/base/types.h"
#include "damath-zero/games/tictactoe/board.h"

using namespace DamathZero;
using namespace DamathZero::Games;

struct GameId : Base::Id<GameId> {
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
    GameId id;
  };
};

struct Move {
  struct Request {
    GameId id;
    i32 cell;
  };
};

struct Response {
  GameId id;
  TicTacToe::Board board;
  Core::Player player;
  Core::GameResult result;
};

class GameServer {
 public:
  auto start() -> void;

  GameServer(std::string_view hostname = "0.0.0.0", u16 port = 8080)
      : ws_(port, hostname.data()) {}

 private:
  ix::WebSocketServer ws_;
  glz::rpc::server<glz::rpc::method<"new", New::Request, Response>,
                   glz::rpc::method<"get", Get::Request, Response>,
                   glz::rpc::method<"move", Move::Request, Response>>
      rpc_server_;

  GameStorage games_;
};

template <>
struct glz::meta<Response> {
  using T = Response;
  static constexpr auto value =
      object(&T::id, &T::board, &T::player, &T::result);
};
