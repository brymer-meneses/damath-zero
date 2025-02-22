from max.tensor import Tensor

trait Network:
  fn inference(self, input: Tensor[DType.float64]) -> (Float64, Tensor[DType.float64]): 
    ...