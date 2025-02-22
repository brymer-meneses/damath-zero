from python import Python

fn main() raises -> None:
  var np = Python.import_module("numpy")
  print(np.arange(0, 10))