#pragma once

#include <ixwebsocket/IXWebSocketServer.h>

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>
#include <memory>
#include <print>

#include "damath-zero/base/storage.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/trainer.h"
#include "damath-zero/games/connect2/board.h"
#include "damath-zero/games/connect2/network.h"
#include "damath-zero/server/rpc.h"

namespace DamathZero::Games::Connect2 {

struct GameId : Base::Id {
  using Id::Id;
};

struct GameStorage : Base::Storage<GameId, Core::Game<Connect2::Board>> {
  using Storage::Storage;
};

struct Context {
  GameStorage games;
  std::shared_ptr<Core::Trainer<Connect2::Board, Connect2::Network>> trainer;
};

struct Response {
  GameId id;
  Connect2::Board board;
  Core::Player player;
  Core::GameResult result;
};

struct New {
  struct Request {};
  using Response = Response;

  static constexpr glz::string_literal method = "new";

  static auto handle(std::shared_ptr<Context> context, const Request& request)
      -> glz::expected<Response, glz::rpc::error> {
    auto id = context->games.create();
    auto& game = context->games.get(id);
    Core::Player player = Core::Player::First;

    return Response{
        .id = id,
        .board = game.board(),
        .player = game.to_play(),
        .result = game.board().get_result(player),
    };
  }
};

struct Get {
  struct Request {
    GameId id;
  };
  using Response = Response;

  static constexpr glz::string_literal method = "get";

  static auto handle(std::shared_ptr<Context> context, const Request& request)
      -> glz::expected<Response, glz::rpc::error> {
    auto id = context->games.create();
    auto& game = context->games.get(id);
    Core::Player player = Core::Player::First;

    return Response{
        .id = id,
        .board = game.board(),
        .player = game.to_play(),
        .result = game.board().get_result(player),
    };
  }
};

struct Move {
  struct Request {
    GameId id;
    i32 cell;
  };
  using Response = Response;

  static constexpr glz::string_literal method = "move";

  static auto handle(std::shared_ptr<Context> context, const Request& request)
      -> glz::expected<Response, glz::rpc::error> {
    auto id = GameId{request.id};
    auto& game = context->games.get(id);
    Core::Player player = Core::Player::First;

    if (game.board().is_terminal(game.to_play())) {
      return Response{
          .id = id,
          .board = game.board(),
          .player = game.to_play(),
          .result = game.board().get_result(player),
      };
    };

    game.apply(Core::ActionId{request.cell});

    if (not game.is_terminal()) {
      auto mcts = Core::MCTS(context->trainer->config);
      auto root_id = mcts.run(game.to_play(), game.board(), game.history_size(),
                              context->trainer->networks.get_latest());
      auto action = mcts.nodes().get(root_id).action_taken;
      game.apply(action);
    }

    return Response{
        .id = id,
        .board = game.board(),
        .player = game.to_play(),
        .result = game.board().get_result(player),
    };
  }
};

struct Server : Rpc::Rpc<Context, New, Get, Move> {
  using Rpc::Rpc;

  Server(std::string_view hostname = "0.0.0.0", u16 port = 8080,
         std::shared_ptr<Core::Trainer<Connect2::Board, Connect2::Network>>
             trainer = std::make_shared<
                 Core::Trainer<Connect2::Board, Connect2::Network>>(
                 Core::Config{}))
      : ws_{port, hostname.data()},
        trainer_(trainer),
        Rpc(Context{
            .trainer = trainer,
        }) {}

  auto start() -> void {
    ws_.setOnClientMessageCallback(
        [this](std::shared_ptr<ix::ConnectionState> connectionState,
               ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
          if (msg->type == ix::WebSocketMessageType::Message) {
            auto result = call(msg->str);
            webSocket.send(result, msg->binary);
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

    trainer_->train();
    ws_.wait();
  }

 private:
  ix::WebSocketServer ws_;
  std::shared_ptr<Core::Trainer<Connect2::Board, Connect2::Network>> trainer_;
};

}  // namespace DamathZero::Games::Connect2

template <>
struct glz::meta<DamathZero::Games::Connect2::Response> {
  using T = DamathZero::Games::Connect2::Response;
  static constexpr auto value =
      object(&T::id, &T::board, &T::player, &T::result);
};
