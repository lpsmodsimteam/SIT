cmake_minimum_required(VERSION 3.12)
project(proto CXX)
include(../../src/CMakeLists.txt)

add_executable(stoplight.o blackboxes/stoplight_driver.cpp)
target_link_libraries(stoplight.o SystemC::systemc)
add_executable(libintersection.so car_generator.cpp intersection.cpp traffic_light.cpp blackboxes/stoplight.cpp)
_sst_compile_link(libintersection.so)
