project(syntatic)

cmake_minimum_required(VERSION 2.6)

add_definitions("-D__GXX_EXPERIMENTAL_CXX0X__ -std=c++0x")

include_directories(${CMAKE_SOURCE_DIR})

add_executable(main main.cpp)

add_executable(tests tests.cpp)
