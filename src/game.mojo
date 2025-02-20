

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

  fn __eq__(self, other: Player) -> Bool:
    return self.__is_first == other.__is_first


@value
@register_passable("trivial")
struct ActionId:
  """
  Generic identifier for an action or a move of a game. Games that implement
  the `Game` trait are responsible for decoding and encoding this information.
  """
  var value: Int

  @staticmethod
  fn invalid() -> ActionId:
    return ActionId { value: -1}

  fn __eq__(self, other: ActionId) -> Bool:
    return self.value == other.value

trait Game:
  fn __moveinit__(out self, owned existing: Self):
    ...

  fn get_valid_moves(ref self) -> ref[self] List[ActionId]:
    ...

  fn is_terminal(self) -> Bool:
    ...

  fn to_play(self) -> Player:
    ...

  fn apply_action(self, action: ActionId) -> None:
    ...


