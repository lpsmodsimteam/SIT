cmake_minimum_required(VERSION 3.12)
project(sock CXX)

add_executable(libtests.so ../../testcases/unit.cpp blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
