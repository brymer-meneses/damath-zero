#pragma once

#include <ixwebsocket/IXWebSocketServer.h>

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>
#include <memory>

#include "damath-zero/base/storage.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/config.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/trainer.h"
#include "damath-zero/games/tictactoe/board.h"
#include "damath-zero/games/tictactoe/network.h"
#include "damath-zero/server/rpc.h"

namespace DamathZero::Games::TicTacToe {

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

struct Context {
  GameStorage games;
  Core::Trainer<TicTacToe::Board, TicTacToe::Network>* trainer;
};

struct Response {
  GameId id;
  TicTacToe::Board board;
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
        .board = game.board,
        .player = player,
        .result = game.board.get_result(player),
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
        .board = game.board,
        .player = player,
        .result = game.board.get_result(player),
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

    if (not game.board.is_terminal(player)) {
      auto mcts = Core::MCTS(context->trainer->config);
      auto root_id =
          mcts.run(player, game.board, context->trainer->networks.get_latest());
      auto action = mcts.nodes().get(root_id).action_taken;
      std::tie(player, board) =
          game.board.apply(game.player, Core::ActionId{request.cell});
    }

    game.board = board;
    game.player = player;

    return Response{
        .id = id,
        .board = board,
        .player = player,
        .result = board.get_result(Core::Player::First),
    };
  }
};

struct Server : Rpc::Rpc<Context, New, Get, Move> {
  using Rpc::Rpc;

  Server(std::string_view hostname = "0.0.0.0", u16 port = 8080)
      : ws_{port, hostname.data()},
        trainer_(Core::Config{}),
        Rpc(Context{
            .trainer = &trainer_,
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

    trainer_.train();
    ws_.wait();
  }

 private:
  ix::WebSocketServer ws_;
  Core::Trainer<TicTacToe::Board, TicTacToe::Network> trainer_;
};

}  // namespace DamathZero::Games::TicTacToe

template <>
struct glz::meta<DamathZero::Games::TicTacToe::Response> {
  using T = DamathZero::Games::TicTacToe::Response;
  static constexpr auto value =
      object(&T::id, &T::board, &T::player, &T::result);
};
