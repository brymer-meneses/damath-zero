#include "damath-zero/server/tictactoe.h"

#include <glaze/util/expected.hpp>
#include <print>

#include "damath-zero/core/board.h"

using namespace DamathZero;

auto GameServer::start() -> void {
  rpc_server_.on<"new">([this](const New::Request& request)
                            -> glz::expected<Response, glz::rpc::error> {
    auto id = games_.create();
    auto& game = games_.get(id);
    Core::Player player = Core::Player::First;

    return Response{
        .id = id,
        .board = game.board,
        .player = player,
        .result = game.board.get_result(player),
    };
  });

  rpc_server_.on<"get">([this](const Get::Request& request)
                            -> glz::expected<Response, glz::rpc::error> {
    auto id = GameId{request.id};
    auto& game = games_.get(id);

    return Response{
        .id = id,
        .board = game.board,
        .player = game.player,
        .result = game.board.get_result(game.player),
    };
  });

  rpc_server_.on<"move">([this](const Move::Request& request)
                             -> glz::expected<Response, glz::rpc::error> {
    auto id = GameId{request.id};
    auto& game = games_.get(id);

    if (game.board.is_terminal(game.player)) {
      return Response{
          .id = id,
          .board = game.board,
          .player = game.player,
          .result = game.board.get_result(game.player),
      };
    };

    auto [player, board] =
        game.board.apply(game.player, Core::ActionId{request.cell});

    game = {board, player};

    return Response{
        .id = id,
        .board = board,
        .player = player,
        .result = board.get_result(player),
    };
  });

  ws_.setOnClientMessageCallback(
      [this](std::shared_ptr<ix::ConnectionState> connectionState,
          ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
              if (msg->type == ix::WebSocketMessageType::Message) {
                  std::println("Received: {}", msg->str);
                  std::println("Sent: {}", rpc_server_.call(msg->str));

                  webSocket.send(rpc_server_.call(msg->str), msg->binary);
              }
          });

  auto res = ws_.listen();
  std::println("Listening on ws://{}:{}", ws_.getHost(), ws_.getPort());
  if (!res.first) {
    std::println("Failed to listen on ws://{}:{}", ws_.getHost(),
                 ws_.getPort());
    std::println("Error: {}", res.second);
    return;
  }

  ws_.start();
  ws_.wait();
}
