option(USE_SYSTEM_GRPC "Use system installed gRPC" ON)
if(USE_SYSTEM_GRPC)
  find_package(gRPC CONFIG REQUIRED)
else()
  FetchContent_Declare(gRPC
    GIT_REPOSITORY https://github.com/grpc/grpc
    GIT_TAG master)
  FetchContent_MakeAvailable(gRPC)
endif()