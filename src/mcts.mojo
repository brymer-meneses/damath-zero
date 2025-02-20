from node import NodeStorage, NodeId, NodeData
from game import Game, ActionId
from max.tensor import Tensor

import math


@value
struct Config:
  var c_base: Float64
  var c_init: Float64
  var num_simulations: Int

  fn __init__(mut self, 
              num_simulations: Int,
              c_base: Float64, 
              c_init: Float64) -> None:

    self.c_base = c_base
    self.c_init = c_init
    self.num_simulations = num_simulations

trait Network(Copyable):
  fn inference(self, input: Tensor[DType.float64]) -> (Float64, Tensor[DType.float64]): 
    ...

struct MCTS[Game: Game, Network: Network]:
  var storage: NodeStorage
  var network: Network
  var config: Config
  var game: Game

  fn __init__(mut self, config: Config, owned game: Game, ref network: Network):
    self.storage = NodeStorage()
    self.config = config
    self.game = game^
    self.network = network

  fn run(mut self) -> None:
    var root = self.storage.push(NodeData(ActionId.invalid(), prior=0))
    var value = self.expand_node(root)

    for _ in range(self.config.num_simulations):
      var node = root
      var game = self.game.clone()
      var path = List[NodeId]()

      while self.storage[node].is_expanded():
        pass
      pass

  fn expand_node(mut self, node_id: NodeId) -> Float64:
    var legal_actions = self.game.get_valid_moves()
    var inference = self.network.inference(self.game.make_image())

    var value = inference[0]
    var policy = inference[1]

    var node = self.storage[node_id];
    var player = node.played_by.next()
    
    for action in legal_actions:
      var action_id = action[]
      var child = self.storage.push(NodeData(action_id,  played_by=player, prior=policy[action_id.value()]))
      node.children.append(child)
      
    return value

  fn compute_ucb_score(self, parent_id: NodeId, child_id: NodeId) -> Float64:
    var parent = self.storage[parent_id]
    var child = self.storage[child_id]

    var pb_c = math.log((parent.visit_count + self.config.c_base + 1) / (self.config.c_base) + self.config.c_init)
    pb_c *= math.sqrt(parent.visit_count / (child.visit_count + 1))

    var prior_score = pb_c * child.prior
    var value_score = child.value_sum

    return prior_score + value_score

  fn select_action(self, root: NodeId) -> ActionId:
    return ActionId(-1)

  fn select_child(self, node_id: NodeId) -> NodeId:
    return NodeId(-1)

