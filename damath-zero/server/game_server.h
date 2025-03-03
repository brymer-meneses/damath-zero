#pragma once

#include <httplib.h>

#include <format>
#include <glaze/glaze.hpp>
#include <ranges>
#include <string_view>

#include "damath-zero/core/game.h"
#include "damath-zero/core/network.h"
#include "damath-zero/games/tictactoe/board.h"

namespace DamathZero::Server {

template <Core::Game Game, Core::Network Network>
auto setup_server(httplib::Server& server, Core::Config config,
                  std::string_view name) -> void {}

}  // namespace DamathZero::Server
