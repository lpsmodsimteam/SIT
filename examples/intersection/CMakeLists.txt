cmake_minimum_required(VERSION 3.12)
project(intersection CXX)
include(../../src/CMakeLists.txt)

add_executable(stoplight.o blackboxes/stoplight_driver.cpp)
target_link_libraries(stoplight.o SystemC::systemc)

add_executable(libintersection.so intersection.cpp car_generator.cpp traffic_light.cpp)
_sst_compile_link(libintersection.so)
