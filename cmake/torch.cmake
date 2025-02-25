option(TORCH_BACKEND "The torch backend to use." "CPU")
option(USE_SYSTEM_TORCH "Use system installed Torch" ON)
if(USE_SYSTEM_TORCH)
  find_package(Torch REQUIRED)
else()
  FetchContent_Declare(gRPC
    GIT_REPOSITORY https://github.com/grpc/grpc
    GIT_TAG master)
  FetchContent_MakeAvailable(gRPC)
endif()