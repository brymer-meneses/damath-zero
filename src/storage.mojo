
trait StorageId(Copyable, Movable):
  fn __init__(inout self: Self, value: Int) -> None: ...
  fn is_valid(self) -> Bool: ...
  fn value(self) -> Int: ... 


struct Storage[Element: CollectionElement, ElementId: StorageId]:
  var data: List[Element]

  fn __init__(inout self) -> None:
    self.data = List[Element]()

  fn __getitem__(ref self, id: ElementId) -> ref[self.data] Element:
    debug_assert(id.is_valid())
    return self.data[id.value()]

  fn push(mut self, owned data: Element) -> ElementId:
    self.data.append(data)
    return ElementId(len(self.data)-1)
    
