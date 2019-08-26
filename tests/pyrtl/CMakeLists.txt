cmake_minimum_required(VERSION 3.12)
project(pyrtl CXX)
include(../CMakeLists.txt)

#if ("${IPC}" STREQUAL "SOCKS")
    add_subdirectory(socks)
#elseif ("${IPC}" STREQUAL "ZMQ")
#    add_subdirectory(zmq)
#endif ()
