

@value
@register_passable("trivial")
struct Player:

  var __is_first: Bool

  fn __init__(inout self)  -> None:
    self.__is_first = True

  fn __eq__(self, other: Player) -> Bool:
    return other.__is_first == self.__is_first


  fn is_first(self) -> Bool:
    return self.__is_first

  fn is_second(self) -> Bool:
    return not self.__is_first

  fn invert(mut self) -> None:
    self.__is_first = not self.__is_first
    
    
