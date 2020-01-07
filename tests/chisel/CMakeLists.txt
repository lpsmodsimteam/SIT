cmake_minimum_required(VERSION 3.12)
project(chisel CXX)
include(../CMakeLists.txt)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
if ("${IPC}" STREQUAL "SOCK")
    add_subdirectory(sock)
elseif ("${IPC}" STREQUAL "ZMQ")
    add_subdirectory(zmq)
endif ()
