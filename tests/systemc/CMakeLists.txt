cmake_minimum_required(VERSION 3.12)
project(systemc CXX)
include(../CMakeLists.txt)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

add_definitions(-DMSGPACK)

if ("${IPC}" STREQUAL "SOCK")
    add_subdirectory(sock)
elseif ("${IPC}" STREQUAL "ZMQ")
    add_subdirectory(zmq)
endif ()
