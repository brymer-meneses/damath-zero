cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

include(FetchContent)

include(cmake/grpc.cmake)
include(cmake/protobuf.cmake)
include(cmake/torch.cmake)

add_library(damath-zero)
target_sources(damath-zero PRIVATE damath-zero/core/mcts.cpp)
target_compile_features(damath-zero PRIVATE cxx_std_23)
target_include_directories(damath-zero PUBLIC .)
target_link_libraries(damath-zero PUBLIC ${TORCH_LIBRARIES})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS damath-zero.proto)
add_library(damath-zero-proto ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(damath-zero-proto PUBLIC ${Protobuf_LIBRARIES})

add_executable(server)
target_sources(server PRIVATE damath-zero/server/main.cpp)
target_compile_features(server PRIVATE cxx_std_23)
target_link_libraries(server PRIVATE damath-zero damath-zero-proto)