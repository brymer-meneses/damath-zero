#pragma once

#include <torch/torch.h>

#include "damath-zero/base/macros.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Games::TicTacToe {

struct Network : public torch::nn::Module {
  Network()
      : l1(register_module("l1", torch::nn::Linear(9, 64))),
        l2(register_module("l2", torch::nn::Linear(64, 32))),
        value_head(register_module("value_head", torch::nn::Linear(32, 1))),
        policy_head(register_module("policy_head", torch::nn::Linear(32, 9))) {}

  auto forward(torch::Tensor x) -> std::pair<torch::Tensor, torch::Tensor> {
    x = torch::relu(l1->forward(x));
    x = torch::relu(l2->forward(x));

    auto value = value_head->forward(x);
    auto policy = policy_head->forward(x);

    return {value, policy};
  }

  torch::nn::Linear l1, l2, value_head, policy_head;
};

REQUIRE_CONCEPT(Core::Concepts::Network, Network);

}  // namespace DamathZero::Games::TicTacToe
