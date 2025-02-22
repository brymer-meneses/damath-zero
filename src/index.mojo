@value
@register_passable("trivial")
struct IndexBase[tag: StringLiteral]:
  """
  IndexBase is used to disambiguate between similar `IndexLike` objects due to
  the data-oriented design nature of this codebase.

  Mojo doesn't have struct inheritance yet, so this is the best we can do to
  avoid code duplication.
  """

  var index: Int32

  alias Invalid = Self(-1)

  fn __init__(mut self, index: Int32) -> None:
    self.index = index

  fn __str__(read self) -> String:
    return tag + "(" + String(self.index) + ")"

  fn __eq__(read self, read other: Self) -> Bool:
    return self.index == other.index

  fn is_valid(read self) -> Bool:
    return self.index != -1

  fn value(read self) -> Int:
    return Int(self.index)
  

trait IndexLike:
  fn __init__(mut self, index: Int32) -> None: ...
  fn is_valid(read self) -> Bool: ...
  fn value(read self) -> Int: ... 