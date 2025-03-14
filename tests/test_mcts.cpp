#include <gtest/gtest.h>

#include <ostream>
#include <print>

#include "damath-zero/core/config.h"
#include "damath-zero/core/game.h"
#include "damath-zero/core/mcts.h"
#include "damath-zero/core/network.h"
#include "damath-zero/core/node.h"
#include "damath-zero/games/tictactoe/board.h"
#include "damath-zero/games/tictactoe/network.h"

using Board = DamathZero::Games::TicTacToe::Board;
using Network = DamathZero::Games::TicTacToe::Network;

using namespace DamathZero::Core;

TEST(MCTS, ExpandNode) {
  Config config;
  Board board;
  auto network = std::make_shared<UniformNetwork>();
  Player player = Player::First;

  MCTS mcts(config);
  auto& storage = mcts.nodes();
  auto root_id = storage.create(0.0);

  auto value = mcts.expand_node(root_id, player, board, network);
  auto& root = storage.get(root_id);

  EXPECT_TRUE(root.is_expanded());
  EXPECT_EQ(root.played_by, player);
  EXPECT_EQ(root.children.size(), 9);
  EXPECT_EQ(value, 0.5);

  auto i = 0;
  for (auto child_id : root.children) {
    auto& child = storage.get(child_id);

    EXPECT_NE(child.played_by, root.played_by);

    EXPECT_EQ(child.prior, 1.0 / 9);
    EXPECT_EQ(child.action_taken.value(), i++);
    EXPECT_EQ(child.visit_count, 0);
    EXPECT_EQ(child.value_sum, 0);
    EXPECT_FALSE(child.is_expanded());
  }
}
