from node import NodeStorage, NodeId, Node
from game import Game, ActionId, Player
from network import Network
from max.tensor import Tensor
from memory import ArcPointer

import math


@value
struct Config:
  var c_base: Float64
  var c_init: Float64
  var num_simulations: Int
  var num_sampling_moves: Int


struct MCTS[Network: Network]:
  var nodes: NodeStorage
  var config: Config
  var network: ArcPointer[Network]

  fn __init__(mut self, config: Config, network: ArcPointer[Network]):
    self.config = config
    self.network = network
    self.nodes = NodeStorage()

  fn run(mut self, owned game: Game) -> NodeId:
    var root = self.nodes.create(Node(ActionId.Invalid, prior=0))
    _ = self.expand_node(game, root)

    for _ in range(self.config.num_simulations):
      var node_id = root
      var scratch_game = game.clone()
      var search_path = List[NodeId](node_id)

      while self.nodes.get(node_id).is_expanded():
        node_id = self.select_highest_score(node_id)
        var action_id = self.nodes.get(node_id).action_taken
        scratch_game.apply_action(action_id)
        search_path.append(node_id)

      var value = self.expand_node(scratch_game, node_id)
      self.backpropagate(search_path, value, scratch_game.to_play())

    return self.select_highest_visit_count(root)

  fn expand_node(mut self, read game: Game, node_id: NodeId) -> Float64:
    var legal_actions = game.get_valid_moves()
    var inference = self.network[].inference(game.make_image())

    var value = inference[0]
    var policy = inference[1]

    var node = self.nodes.get(node_id)
    var player = node.played_by.next()
    
    for action in legal_actions:
      var action_id = action[]
      var child_id = self.nodes.create(Node(action_id,  played_by=player, prior=policy[action_id.value()]))
      node.children.append(child_id)
    return value

  fn compute_puct(read self, parent_id: NodeId, child_id: NodeId) -> Float64:
    var parent = self.nodes.get(parent_id)
    var child = self.nodes.get(child_id)

    var pb_c = math.log((parent.visit_count + self.config.c_base + 1) / (self.config.c_base) + self.config.c_init)
    pb_c *= math.sqrt(parent.visit_count / (child.visit_count + 1))

    var prior_score = pb_c * child.prior
    var value_score = child.mean_value_score()

    return prior_score + value_score

  fn select_highest_visit_count(self, parent_id: NodeId) -> NodeId:
    var max_visit_count = 0
    var best_child_id = NodeId.Invalid
    for child_id in self.nodes.get(parent_id).children:
      var child = self.nodes.get(child_id[])
      if child.visit_count > max_visit_count:
        max_visit_count = child.visit_count
        best_child_id = child_id[]
    return best_child_id

  fn select_highest_score(self, parent_id: NodeId) -> NodeId:
    var best_child_id = NodeId.Invalid
    var best_score = 0.0
    var parent = self.nodes.get(parent_id)
    debug_assert(parent.is_expanded())

    for child_id in parent.children:
      var score = self.compute_puct(parent_id, child_id[]) 
      if score > best_score:
        best_child_id = child_id[]
    return best_child_id

  fn backpropagate(self, path: List[NodeId], value: Float64, played_by: Player):
    for node_id in path:
      var node = self.nodes.get(node_id[])
      if node.played_by == played_by:
        node.value_sum += value
      else:
        node.value_sum += (1-value)
