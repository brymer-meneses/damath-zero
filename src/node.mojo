from memory import UnsafePointer
from storage import Storage, StorageId
from game import Player, ActionId

import math

@value
struct NodeData:
  var visit_count: Int
  var prior: Float64
  var value_sum: Float64
  var played_by: Player
  var action_taken: ActionId
  var children: List[NodeId]
  

  fn __init__(mut self,  action_taken: ActionId, played_by: Player=Player.first(), prior: Float64=0) -> None:
    self.visit_count = 0
    self.value_sum = 0
    self.prior = prior
    self.played_by = played_by
    self.children = List[NodeId]()
    self.action_taken = action_taken

  fn is_expanded(ref self) -> Bool:
    return len(self.children) > 0


@value
@register_passable("trivial")
struct NodeId(StorageId):
  var index: Int

  fn __init__(mut self, index: Int) -> None:
    self.index = index

  @staticmethod
  fn invalid() -> NodeId:
    return NodeId(-1)

  fn is_valid(self) -> Bool:
    return self.index != -1

  fn value(self) -> Int:
    return self.index

alias NodeStorage = Storage[NodeData, NodeId]

