#include "damath-zero/server/tictactoe.h"

#include <glaze/util/expected.hpp>
#include <print>

using namespace DamathZero;

auto GameServer::listen(std::string_view hostname, u16 port) -> void {
  std::println("Listening on http://{}:{}...", hostname, port);
  if (not http_server_.listen("localhost", port)) {
    std::println("Failed to start server.");
  }
}

GameServer::GameServer() {
  rpc_server_.on<"new">([this](const New::Request& request)
                            -> glz::expected<New::Response, glz::rpc::error> {
    auto id = games_.create();
    auto& game = games_.get(id);
    Core::Player player = Core::Player::First;

    return New::Response{
        .id = id.value(),
        .board = game.board.data(),
        .player = player.value(),
    };
  });

  rpc_server_.on<"move">([this](const Move::Request& request)
                             -> glz::expected<Move::Response, glz::rpc::error> {
    auto& game = games_.get(GameId{request.id});
    auto [player, board] =
        game.board.apply(game.player, Core::ActionId{request.cell});

    game = {board, player};

    return Move::Response{
        .board = board.data(),
        .player = player.value(),
    };
  });

  http_server_.Post("/tictactoe", [this](const httplib::Request& req,
                                         httplib::Response& res) {
    auto response = rpc_server_.call(req.body);
    res.set_content(std::move(response), "application/json");
  });
}