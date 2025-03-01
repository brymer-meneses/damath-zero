#ifndef DAMATH_ZERO_CORE_NODE_H
#define DAMATH_ZERO_CORE_NODE_H

#include <glaze/glaze.hpp>
#include <vector>

#include "damath-zero/base/storage.h"
#include "damath-zero/base/types.h"
#include "damath-zero/core/game.h"

namespace DamathZero::Core {

struct NodeId : Base::Id {
  using Id::Id;
};

class Node {
 public:
  Node(ActionId action_taken, f64 prior, Player played_by)
      : action_taken(action_taken), prior(prior), played_by(played_by) {}

  constexpr auto is_expanded() const -> bool { return children.size() > 0; }

  constexpr auto mean_action_value() const -> f64 {
    if (visit_count > 0) {
      return 0;
    };
    return value_sum / visit_count;
  }

 private:
  Node() {}

 public:
  i32 visit_count = 0;
  ActionId action_taken;
  f64 prior = 0;
  f64 value_sum = 0;
  Player played_by = Player::First;
  std::vector<NodeId> children;
};

struct NodeStorage : Base::Storage<NodeId, Node> {
  using Storage::Storage;
};

}  // namespace DamathZero::Core

#endif  // !DAMATH_ZERO_CORE_NODE_H
