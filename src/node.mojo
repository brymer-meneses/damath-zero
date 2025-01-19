from memory import UnsafePointer
import math

from game import Player

@value
@register_passable("trivial")
struct NodeId:
  var index: Int

  fn __init__(inout self, index: Int) -> None:
    self.index = index

  @staticmethod
  fn invalid() -> NodeId:
    return NodeId(-1)

  fn is_valid(self) -> Bool:
    return self.index != -1

@value
struct NodeData:
  var n: Int
  var q: Float64
  var children: List[NodeId]
  var played_by: Player

struct NodeStorage:
  var data: List[NodeData]

  fn __init__(inout self) -> None:
    self.data = List[NodeData]()

  fn __getitem__(ref self, node: NodeId) -> ref[self.data] NodeData:
    return self.data[node.index]

  fn backpropagate(mut self, path: List[NodeId], value: Float64, played_by: Player) -> None:
    for node_id in path:
      var node = self[node_id[]]
      node.n += 1

      if played_by == node.played_by:
        node.q += value
      else:
        node.q += 1 - value
      
