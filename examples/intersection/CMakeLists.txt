cmake_minimum_required(VERSION 3.12)
project(intersection CXX)
include(../CMakeLists.txt)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# increase default buffer size
add_definitions(-DBUFSIZE=250)

add_executable(traffic_light_fsm.o blackboxes/traffic_light_fsm_driver.cpp)
target_link_libraries(traffic_light_fsm.o SystemC::systemc)

add_executable(libintersection.so intersection.cpp car_generator.cpp traffic_light.cpp)
_sst_compile_link(libintersection.so)
