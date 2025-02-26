cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

include(FetchContent)
include(cmake/torch.cmake)
include(cmake/httplib.cmake)
include(cmake/gtest.cmake)

add_library(damath-zero)
target_compile_features(damath-zero PRIVATE cxx_std_23)
target_sources(damath-zero PRIVATE damath-zero/core/mcts.cpp)
target_include_directories(damath-zero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(damath-zero PUBLIC ${TORCH_LIBRARIES})

add_executable(damath-zero-server)
target_compile_features(damath-zero-server PRIVATE cxx_std_23)
target_sources(damath-zero-server PRIVATE damath-zero/server/main.cpp)
target_link_libraries(damath-zero-server PRIVATE damath-zero httplib::httplib)

add_custom_command(TARGET damath-zero-server POST_BUILD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/damath-zero/app
  COMMAND bun install && bun run build && rm -rf node_modules && rm bun.lock && mv -f dist ${CMAKE_CURRENT_BINARY_DIR}/dist)
  
# Tests
enable_testing()

add_executable(test_runner 
  tests/test_base.cpp
  tests/test_mcts.cpp
)

target_compile_features(test_runner PRIVATE cxx_std_23)
target_link_libraries(test_runner damath-zero GTest::gtest_main)
target_include_directories(test_runner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GTEST_INCLUDE_DIRS})

include(GoogleTest)
gtest_discover_tests(test_runner)

