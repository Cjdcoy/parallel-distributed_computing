# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(odd-even-sort)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (CMAKE_CXX_STANDARD 14)


# The following folder will be included
include_directories(include/ utils/log/include utils/timer/include utils/error/)

set(GCC_OPTIMISATION "-o3")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_OPTIMISATION}")
set(SOURCES src/main.cpp
        src/runner.cpp
        src/odd_even_sort.cpp
        include/odd_even_sort.h
        include/runner.h
        include/main.h
        utils/log/src/logs.cpp
        utils/timer/src/timer.cpp
        utils/log/include/logs.h
        utils/timer/include/timer.h
        utils/error/errors.h)

add_executable(odd-even-sort
        ${SOURCES})

target_link_libraries (odd-even-sort)
