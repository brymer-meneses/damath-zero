
from max.tensor import Tensor
from index import IndexBase

@value
@register_passable("trivial")
struct Player:
  var __is_first: Bool 

  alias First = Self(True)
  alias Second = Self(False)

  fn next(self) -> Player:
    return Player(not self.__is_first)

  fn __eq__(self, other: Player) -> Bool:
    return self.__is_first == other.__is_first


alias ActionId = IndexBase["ActionId"]

trait Game:
  fn clone(self) -> Game: ...

  fn __moveinit__(out self, owned existing: Self):
    ...

  fn get_history(self) -> List[ActionId]:
    ...

  fn get_valid_moves(ref self) -> ref[self] List[ActionId]:
    ...

  fn make_image(ref self) -> Tensor[DType.float64]: 
    ...

  fn is_terminal(self) -> Bool:
    ...

  fn to_play(self) -> Player:
    ...

  fn apply_action(self, action: ActionId) -> None:
    ...


