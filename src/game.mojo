
from max.tensor import Tensor

@value
@register_passable("trivial")
struct Player:
  var __is_first: Bool 

  @staticmethod
  fn first() -> Player:
    return Player { __is_first: True}

  @staticmethod
  fn second() -> Player:
    return Player { __is_first: False}

  fn next(self) -> Player:
    return Player { __is_first: not self.__is_first}

  fn __eq__(self, other: Player) -> Bool:
    return self.__is_first == other.__is_first


@value
@register_passable("trivial")
struct ActionId:
  """
  Generic identifier for an action or a move of a game. Games that implement
  the `Game` trait are responsible for decoding and encoding this information.
  """
  var __value: Int

  @staticmethod
  fn invalid() -> ActionId:
    return ActionId { __value: -1}

  fn value(self) -> Int:
      return self.__value

  fn __eq__(self, other: ActionId) -> Bool:
    return self.__value == other.__value

trait Game:
  fn clone(self) -> Game: ...

  fn __moveinit__(out self, owned existing: Self):
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


