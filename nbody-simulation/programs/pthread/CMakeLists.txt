# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(nbody_pthread)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (CMAKE_CXX_STANDARD 14)


# Add pthread
find_package (Threads REQUIRED)
find_package(X11 REQUIRED)

# The following folder will be included
include_directories(include/ utils/log/include ../utils/ ../utils/timer/include utils/error/)

set(GCC_OPTIMISATION "-o3")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_OPTIMISATION}")
set(SOURCES src/main.cpp
        include/main.h
        ../utils/x11-helpers.h
        ../utils/body.h
        ../utils/timer/src/timer.cpp
        ../utils/timer/include/timer.h)

add_executable(nbody_pthread
        ${SOURCES})

target_link_libraries (nbody_pthread ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES})
