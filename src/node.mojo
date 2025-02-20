from memory import UnsafePointer
import math

from game import Player, ActionId

@value
struct NodeData:
  var visit_count: Int
  var prior: Float64
  var value_sum: Float64
  var played_by: Player
  var action_taken: ActionId
  var children: List[NodeId]
  

  fn __init__(inout self, prior: Float64, action_taken: ActionId) -> None:
    self.visit_count = 0
    self.value_sum = 0
    self.prior = prior
    self.children = List[NodeId]()
    self.played_by = Player.first()
    self.action_taken = action_taken

  fn __init__(inout self, action_taken: ActionId) -> None:
    self.visit_count = 0
    self.value_sum = 0
    self.prior = 0
    self.children = List[NodeId]()
    self.played_by = Player.first()
    self.action_taken = action_taken

  fn mean_action_value(self) -> Float64:
    if self.visit_count == 0:
      return 0
    return self.value_sum / self.visit_count

  fn is_expanded(self) -> Bool:
    return len(self.children) > 0

@value
@register_passable("trivial")
struct NodeId:
  alias Invalid = NodeId(-1)

  var index: Int

  fn __init__(inout self, index: Int) -> None:
    self.index = index

  fn is_valid(self) -> Bool:
    return self.index != -1

struct NodeStorage:
  var data: List[NodeData]

  fn __init__(inout self) -> None:
    self.data = List[NodeData]()

  fn __getitem__(ref self, node_id: NodeId) -> ref[self.data] NodeData:
    debug_assert(node_id.is_valid())

    return self.data[node_id.index]

  fn new(mut self, prior: Float64, action_taken: ActionId) -> ref[self.data] NodeData:
    self.data.append(NodeData(prior, action_taken))
    return self.data[-1]
    
