cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

include(FetchContent)
include(cmake/glaze.cmake)
include(cmake/gtest.cmake)
include(cmake/ixwebsocket.cmake)
include(cmake/torch.cmake)

add_library(damath-zero)
target_compile_features(damath-zero PRIVATE cxx_std_23)
target_sources(damath-zero PRIVATE damath-zero/core/mcts.cpp)
target_include_directories(damath-zero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(damath-zero PUBLIC ${TORCH_LIBRARIES} glaze::glaze)

add_executable(damath-zero-server)
target_compile_features(damath-zero-server PRIVATE cxx_std_23)
target_sources(damath-zero-server PRIVATE
               damath-zero/server/main.cpp
               damath-zero/games/tictactoe/board.cpp
               damath-zero/server/tictactoe.cpp)
target_link_libraries(damath-zero-server PRIVATE damath-zero ixwebsocket)

add_executable(tictactoe
  damath-zero/games/tictactoe/train.cpp
  damath-zero/games/tictactoe/board.cpp
)
target_compile_features(tictactoe PRIVATE cxx_std_23)
target_link_libraries(tictactoe PRIVATE damath-zero)

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
