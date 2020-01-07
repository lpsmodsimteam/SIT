cmake_minimum_required(VERSION 3.12)
project(sock CXX)

add_executable(ram.o blackboxes/ram_driver.cpp)
target_link_libraries(ram.o SystemC::systemc)

add_executable(libtests.so ../../testcases/unit.cpp blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
