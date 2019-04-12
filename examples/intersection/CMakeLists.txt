cmake_minimum_required(VERSION 3.12)
project(proto CXX)
include(../../src/CMakeLists.txt)

add_executable(stoplight.o blackboxes/stoplight_sock_driver.cpp)
target_link_libraries(stoplight.o SystemC::systemc)
