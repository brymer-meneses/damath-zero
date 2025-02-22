
from index import IndexLike

struct Storage[Element: CollectionElement, ElementId: IndexLike]:
  var data: List[Element]

  fn __init__(mut self) -> None:
    self.data = List[Element]()

  fn get(ref self, id: ElementId) -> ref[self.data] Element:
    debug_assert(id.is_valid())
    return self.data[id.value()]

  fn create(mut self, owned data: Element) -> ElementId:
    self.data.append(data)
    return ElementId(len(self.data)-1)