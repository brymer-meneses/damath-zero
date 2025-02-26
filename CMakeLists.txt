cmake_minimum_required(VERSION 3.31)
project(damath-zero LANGUAGES CXX)

include(FetchContent)
include(cmake/torch.cmake)
include(cmake/httplib.cmake)

add_library(damath-zero)
target_compile_features(damath-zero PRIVATE cxx_std_23)
target_sources(damath-zero PRIVATE damath-zero/core/mcts.cpp)
target_include_directories(damath-zero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(damath-zero PUBLIC ${TORCH_LIBRARIES})

add_executable(damath-zero-server)
target_compile_features(damath-zero-server PRIVATE cxx_std_23)
target_sources(damath-zero-server PRIVATE damath-zero/server/main.cpp)
target_link_libraries(damath-zero-server PRIVATE damath-zero httplib::httplib)

add_custom_command(TARGET damath-zero-server PRE_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/damath-zero/app $<TARGET_FILE_DIR:damath-zero-server>)