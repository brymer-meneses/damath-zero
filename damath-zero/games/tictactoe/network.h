#ifndef DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
#define DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/macros.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Games::TicTacToe {

class Network {
 public:
  auto inference(torch::Tensor) -> torch::Tensor;
  auto save() const -> void;
};

REQUIRE_CONCEPT(Core::Network, Network);

}  // namespace DamathZero::Games::TicTacToe

#endif  // !DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
