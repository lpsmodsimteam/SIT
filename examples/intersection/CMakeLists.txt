cmake_minimum_required(VERSION 3.12)
project(intersection CXX)
include(../CMakeLists.txt)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)
# increase buffer size (minimum is 22)
add_definitions(-DBUFSIZE=25)

if (${HYBRID})

    add_executable(libintersection.so intersection.cpp car_generator.cpp traffic_light.cpp demo/pyrtl/traffic_light.cpp)

else ()

    add_executable(libintersection.so intersection.cpp car_generator.cpp traffic_light.cpp)

endif ()

_sst_compile_link(libintersection.so)
add_executable(traffic_light_fsm.o blackboxes/traffic_light_fsm_driver.cpp)
target_link_libraries(traffic_light_fsm.o SystemC::systemc)
