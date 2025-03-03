#include <httplib.h>

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>
#include <print>

#include "damath-zero/games/tictactoe/board.h"

using namespace DamathZero::Games;
using namespace DamathZero::Core;

class New {
 public:
  struct Request {};

  struct Response {
    std::array<i8, 9> board;
  };
};

class Server {
 public:
  auto listen(std::string_view hostname = "localhost", u16 port = 8080)
      -> bool {
    std::println("Listening on http://{}:{}...", hostname, port);
    return http_server_.listen("localhost", port);
  }

  Server() {
    server.on<"new">([](const New::Request& request)
                         -> glz::expected<New::Response, glz::rpc::error> {
      return New::Response{
          .board = {0, 0, 0, 0, 0, 0, 0, 0, 0},
      };
    });

    http_server_.Post(
        "/new", [this](const httplib::Request& req, httplib::Response& res) {
          auto response = server.call(req.body);
          res.set_content(std::move(response), "application/json");
        });
  }

 private:
  httplib::Server http_server_;
  glz::rpc::server<glz::rpc::method<"new", New::Request, New::Response>> server;

  TicTacToe::Board board;
  Player player = Player::First;
};

auto main(int argc, char** argv) -> int {
  Server server;

  if (not server.listen()) {
    std::println("Failed to start server.");
    return 1;
  }

  return 0;
}
