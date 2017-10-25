cmake_minimum_required(VERSION 3.8)
project(tsp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp stack.cpp graphs.h graphs.cpp)
add_executable(tsp ${SOURCE_FILES})
