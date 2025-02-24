
set(TORCH_BACKEND "CPU" CACHE STRING "The torch backend to use.")

# TODO: Use FetchContent for downloading torch
find_package(Torch REQUIRED)

if (NOT Torch_FOUND)
  message(STATUS "Found LibTorch")
endif()

