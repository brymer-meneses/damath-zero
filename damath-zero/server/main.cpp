#include <httplib.h>

#include <print>

#include "damath-zero/games/tictactoe/game.h"
#include "damath-zero/games/tictactoe/network.h"
#include "damath-zero/server/game_server.h"

auto main(int argc, char** argv) -> int {
  auto srv = httplib::Server{};
  std::vector<DamathZero::Games::TicTacToe> games;

  // auto path = std::string_view{argv[0]};
  // if (path.ends_with("server"))
  //   std::filesystem::current_path(path.substr(0, path.find_last_of("/\\")));

  // if (!srv.set_mount_point("/", std::filesystem::current_path() / "dist"))
  //   std::print("Failed to set mount point...\n");

  auto config = DamathZero::Core::Config{};
  DamathZero::Server::setup_server<DamathZero::Games::TicTacToe,
                                   DamathZero::Games::TicTacToeNetwork>(
      srv, config, "tictactoe");

  std::print("Listening on http://0.0.0.0:80\n");
  if (!srv.listen("0.0.0.0", 80))
    std::print("Failed to listen...\n");

  return 0;
}