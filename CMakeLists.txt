cmake_minimum_required(VERSION 3.8)
project(tsp)


set(CMAKE_CXX_STANDARD 11)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(SOURCE_FILES main.cpp stack.cpp graphs.h graphs.cpp)
add_executable(tsp ${SOURCE_FILES})
