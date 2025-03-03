#ifndef DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
#define DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H

#include <torch/torch.h>

#include "damath-zero/base/macros.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Games::TicTacToe {

struct Network : torch::nn::Module {
  Network() {
    l1 = register_module("fc1", torch::nn::Linear(9, 16));
    l2 = register_module("fc2", torch::nn::Linear(16, 32));
    policy_head = register_module("fc3", torch::nn::Linear(32, 9));
    value_head = register_module("fc3", torch::nn::Linear(32, 1));
  };

  auto forward(torch::Tensor x) -> std::pair<torch::Tensor, torch::Tensor> {
    x = l1->forward(x);
    x = l2->forward(x);
    auto policy = policy_head->forward(x);
    auto value = value_head->forward(x);
    return {value, policy};
  }

  torch::nn::Linear l1{nullptr};
  torch::nn::Linear l2{nullptr};
  torch::nn::Linear policy_head{nullptr};
  torch::nn::Linear value_head{nullptr};
};

REQUIRE_CONCEPT(Core::Network, Network);

}  // namespace DamathZero::Games::TicTacToe

#endif  // !DAMATH_ZERO_GAMES_TICTACTOE_NETWORK_H
