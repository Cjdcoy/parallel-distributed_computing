# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(nbody_openmp)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (CMAKE_CXX_STANDARD 14)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
# Add openmp
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

add_executable(nbody_openmp
        ${SOURCES})

target_link_libraries (nbody_openmp ${X11_LIBRARIES})
