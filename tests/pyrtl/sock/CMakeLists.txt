cmake_minimum_required(VERSION 3.12)
project(sock CXX)

add_executable(libtests.so ../../unit.cpp ${CMAKE_BINARY_DIR}/blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
