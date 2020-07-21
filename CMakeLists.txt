cmake_minimum_required (VERSION 3.11)
project(padosuso CXX)


set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O1")


set (EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set (LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")


include_directories("include")
include_directories("/usr/include/rtmidi/")

add_subdirectory("src")


