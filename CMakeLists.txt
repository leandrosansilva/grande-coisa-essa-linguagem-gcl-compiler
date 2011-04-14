project(grande-coisa-essa-linguagem)

cmake_minimum_required(VERSION 2.6)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(src)
add_subdirectory(1pass-red-dragon)
add_subdirectory(tests)
add_subdirectory(sandbox)
