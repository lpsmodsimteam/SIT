cmake_minimum_required(VERSION 3.12)
project(verilog CXX)
include(../CMakeLists.txt)

if ("${IPC}" STREQUAL "SOCK")
    add_subdirectory(sock)
elseif ("${IPC}" STREQUAL "ZMQ")
    add_subdirectory(zmq)
endif ()
