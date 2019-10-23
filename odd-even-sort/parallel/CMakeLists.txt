# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(odd-even-sort)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (CMAKE_CXX_STANDARD 14)


# Add pthread
find_package (Threads)
find_package(MPI REQUIRED)
# add this line only when you are using openmpi which has a different c++ bindings
add_definitions(-DOMPI_SKIP_MPICXX)

# The following folder will be included
include_directories(include/ utils/log/include utils/timer/include utils/error/ ${MPI_INCLUDE_PATH})

set(GCC_OPTIMISATION "-o3")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_OPTIMISATION}")
set(SOURCES src/main.cpp
        utils/log/src/logs.cpp
        utils/timer/src/timer.cpp
        include/main.h
        utils/log/include/logs.h
        utils/timer/include/timer.h
        utils/error/errors.h
        src/runner.cpp
        include/runner.h
        src/pthread_odd_even_sort.cpp
        include/pthread_odd_even_sort.h
        src/mpi_odd_even_sort.cpp
        include/mpi_odd_even_sort.h)

add_executable(odd-even-sort
        ${SOURCES})

target_link_libraries (odd-even-sort ${CMAKE_THREAD_LIBS_INIT} ${MPI_LIBRARIES})
