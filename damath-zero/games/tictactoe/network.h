#pragma once

#include <torch/torch.h>

#include "damath-zero/base/macros.h"
#include "damath-zero/core/network.h"

namespace DamathZero::Games::TicTacToe {

struct Network : torch::nn::Module {
  Network() {
    l1 = register_module("l1", torch::nn::Linear(9, 16));
    b1 = register_module("b1", torch::nn::BatchNorm1d(16));
    l2 = register_module("l2", torch::nn::Linear(16, 32));
    b2 = register_module("b2", torch::nn::BatchNorm1d(32));
    policy_head = register_module("policy_head", torch::nn::Linear(32, 9));
    value_head = register_module("value_head", torch::nn::Linear(32, 1));
  };

  auto forward(torch::Tensor x) -> std::tuple<torch::Tensor, torch::Tensor> {
    x = l1->forward(x);
    x = torch::relu(x);
    // x = b1->forward(x);
    x = l2->forward(x);
    x = torch::relu(x);
    // x = b2->forward(x);
    auto policy = torch::softmax(policy_head->forward(x), -1);
    auto value = torch::tanh(value_head->forward(x));
    return {value, policy};
  }

  torch::nn::Linear l1{nullptr};
  torch::nn::Linear l2{nullptr};
  torch::nn::BatchNorm1d b1{nullptr};
  torch::nn::BatchNorm1d b2{nullptr};
  torch::nn::Linear policy_head{nullptr};
  torch::nn::Linear value_head{nullptr};
};

REQUIRE_CONCEPT(Core::Network, Network);

}  // namespace DamathZero::Games::TicTacToe
