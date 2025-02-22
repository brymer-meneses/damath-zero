from memory import UnsafePointer
from storage import Storage
from game import Player, ActionId

from index import IndexBase

import math

@value
struct Node:
  var visit_count: Int
  var prior: Float64
  var value_sum: Float64
  var played_by: Player
  var action_taken: ActionId
  var children: List[NodeId]
  

  fn __init__(mut self,  action_taken: ActionId, played_by: Player=Player.First, prior: Float64=0) -> None:
    self.visit_count = 0
    self.value_sum = 0
    self.prior = prior
    self.played_by = played_by
    self.children = List[NodeId]()
    self.action_taken = action_taken

  fn is_expanded(ref self) -> Bool:
    return len(self.children) > 0

  fn mean_value_score(read self) -> Float64:
    if self.visit_count == 0:
      return 0
    return self.value_sum / self.visit_count


alias NodeId = IndexBase["NodeId"]

alias NodeStorage = Storage[Node, NodeId]

