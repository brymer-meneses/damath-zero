#pragma once

#include <httplib.h>

#include <format>
#include <glaze/glaze.hpp>
#include <ranges>
#include <string_view>

#include "damath-zero/core/game.h"
#include "damath-zero/core/network.h"
#include "damath-zero/games/tictactoe/game.h"

namespace DamathZero::Server {

template <Core::Game Game, Core::Network Network>
auto setup_server(httplib::Server& server, Core::Config config,
                  std::string_view name) -> void {
  Games::TicTacToe::Game game;

  //   server.Get(std::format("/{}/predict", name),
  //              [context](const httplib::Request& req, httplib::Response& res)
  //              {
  //                      t_content("{}", "application/json");
  //              });

  server.Get(std::format("/{}/get_legal_actions", name),
             [game](const httplib::Request& req, httplib::Response& res) {
               auto action_id_to_u64 = [](auto action) {
                 return static_cast<u64>(action.value());
               };

               auto legal_actions = game.get_legal_actions() |
                                    std::views::transform(action_id_to_u64) |
                                    std::ranges::to<std::vector<u64>>();

               res.set_content(glz::write_json(legal_actions).value_or("error"),
                               "application/json");
             });
}

}  // namespace DamathZero::Server
