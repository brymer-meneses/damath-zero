#ifndef DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
#define DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/core/network.h"

namespace DamathZero::Games {

class TicTacToeNetwork {
 public:
  auto inference(torch::Tensor t) -> std::pair<f64, torch::Tensor>;
};

static_assert(Core::Network<TicTacToeNetwork>);

}  // namespace DamathZero::Games

#endif  // !DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
