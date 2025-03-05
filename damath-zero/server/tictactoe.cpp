#include "damath-zero/server/tictactoe.h"

#include <glaze/util/expected.hpp>
#include <print>

#include "damath-zero/core/board.h"

using namespace DamathZero;

auto GameServer::listen(std::string_view hostname, u16 port) -> void {
  std::println("Listening on http://{}:{}...", hostname, port);
  if (not http_server_.listen(hostname.data(), port)) {
    std::println("Failed to start server.");
  }
}

GameServer::GameServer() {
  rpc_server_.on<"new">([this](const New::Request& request)
                            -> glz::expected<Response, glz::rpc::error> {
    auto id = games_.create();
    auto& game = games_.get(id);
    Core::Player player = Core::Player::First;

    return Response{
        .id = id.value(),
        .board = game.board.data(),
        .player = player.value(),
        .result = static_cast<i8>(game.board.get_result(player)),
    };
  });

  rpc_server_.on<"get">([this](const Get::Request& request)
                            -> glz::expected<Response, glz::rpc::error> {
    auto& game = games_.get(GameId{request.id});

    return Response{
        .id = request.id,
        .board = game.board.data(),
        .player = game.player.value(),
        .result = static_cast<i8>(game.board.get_result(game.player)),
    };
  });

  rpc_server_.on<"move">([this](const Move::Request& request)
                             -> glz::expected<Response, glz::rpc::error> {
    auto& game = games_.get(GameId{request.id});

    if (game.board.is_terminal(game.player)) {
      return Response{
          .id = request.id,
          .board = game.board.data(),
          .player = game.player.value(),
          .result = static_cast<i8>(game.board.get_result(game.player)),
      };
    };

    auto [player, board] =
        game.board.apply(game.player, Core::ActionId{request.cell});

    game = {board, player};

    return Response{
        .id = request.id,
        .board = board.data(),
        .player = player.value(),
        .result = static_cast<i8>(board.get_result(player)),
    };
  });

  http_server_.Post("/tictactoe", [this](const httplib::Request& req,
                                         httplib::Response& res) {
    auto response = rpc_server_.call(req.body);
    res.set_content(std::move(response), "application/json");
  });
}
