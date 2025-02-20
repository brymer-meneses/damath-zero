from node import NodeStorage, NodeId
from game import Game, ActionId


@value
struct Config:
  var c_base: Float64
  var c_init: Float64
  var num_simulations: Int

  fn __init__(inout self, 
              num_simulations: Int,
              c_base: Float64, 
              c_init: Float64) -> None:

    self.c_base = 19652
    self.c_init = 1.25
    self.num_simulations = num_simulations


struct MCTS[Game: Game]:
  var node_storage: NodeStorage
  var config: Config
  var game: Game

  fn __init__(inout self, config: Config, owned game: Game):
    self.node_storage = NodeStorage()
    self.config = config
    self.game = game^


  fn run(mut self) -> None:
    var root = self.node_storage.new(0, ActionId.invalid())

    pass


  fn select_child(self, node: NodeId) -> None:
    """Select child of `node` with the best ucb score."""
    pass

  fn ucb_score(self, parent: NodeId, child: NodeId) -> None:
    pass

    

