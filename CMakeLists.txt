cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent) 

include(cmake/fmt.cmake)
include(cmake/googletest.cmake)
include(cmake/torch.cmake)

include_directories(.)

add_library(base 
  damath-zero/base/types.h
  damath-zero/base/storage.h
  damath-zero/base/id.h
)

add_library(core
  damath-zero/core/mcts.cpp
  damath-zero/core/mcts.h
  damath-zero/core/network.h
  damath-zero/core/node.h
  damath-zero/core/game.h
)

set_target_properties(core PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(base PROPERTIES LINKER_LANGUAGE CXX)

add_executable(server damath-zero/server/main.cpp)

target_include_directories(core PUBLIC ${TORCH_INCLUDE_DIRS})
target_link_libraries(core PUBLIC ${TORCH_LIBRARY})

target_link_libraries(server PRIVATE core base)

