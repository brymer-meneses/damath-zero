#include <gtest/gtest.h>

#include "damath-zero/base/id.h"
#include "damath-zero/base/storage.h"

struct Node {
  i32 num;
  Node(i32 num) : num(num) {}
};

struct NodeId : DamathZero::Base::Id {
  using Id::Id;
};

struct Nodes : DamathZero::Base::Storage<NodeId, Node> {
  using Storage::Storage;
};

TEST(Base, Storage) {
  Nodes nodes;
  auto node_id1 = nodes.create(1);
  auto node_id2 = nodes.create(2);

  EXPECT_EQ(nodes.get(node_id1).num, 1);
  EXPECT_EQ(nodes.get(node_id2).num, 2);
}

TEST(Base, Id) {
  auto invalid_id = NodeId::Invalid;
  auto valid_id = NodeId(1);

  EXPECT_EQ(invalid_id.is_valid(), false);
  EXPECT_EQ(valid_id.is_valid(), true);
}
