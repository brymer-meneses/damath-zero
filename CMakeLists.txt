cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

include(FetchContent)

include(cmake/grpc.cmake)
include(cmake/protobuf.cmake)
include(cmake/torch.cmake)

add_library(damath-zero)
target_compile_features(damath-zero PRIVATE cxx_std_23)
target_sources(damath-zero PRIVATE damath-zero/core/mcts.cpp)
target_include_directories(damath-zero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(damath-zero PUBLIC ${TORCH_LIBRARIES})

add_executable(damath-zero-server)
target_compile_features(damath-zero-server PRIVATE cxx_std_23)
target_sources(damath-zero-server PRIVATE damath-zero/server/main.cpp)
target_link_libraries(damath-zero-server PRIVATE damath-zero damath-zero-proto-c++)

add_library(damath-zero-proto-c++ damath-zero.proto)
protobuf_generate(
  TARGET damath-zero-proto-c++
  LANGUAGE cpp)
protobuf_generate(
  TARGET damath-zero-proto-c++
  LANGUAGE grpc
  PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
  GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)
target_include_directories(damath-zero-proto-c++ PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(damath-zero-proto-c++ PUBLIC
  gRPC::grpc++
  gRPC::grpc++_reflection
  absl::flags_parse)