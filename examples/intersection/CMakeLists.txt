cmake_minimum_required(VERSION 3.12)
project(intersection CXX)
include(../../tests/CMakeLists.txt)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

# increase buffer size (minimum is 22)
add_definitions(-DBUFSIZE=15)

if (${HYBRID})

    add_executable(libintersection.so intersection.cpp car_generator.cpp traffic_light.cpp pyrtl/traffic_light.cpp)

else ()

    add_executable(
        libintersection.so
        intersection.cpp
        car_generator.cpp
        systemc/blackboxes/traffic_light_fsm_comp.cpp
        traffic_light.cpp
    )

endif ()

_sst_compile_link(libintersection.so)
add_executable(traffic_light_fsm.o systemc/blackboxes/traffic_light_fsm_driver.cpp)
target_link_libraries(traffic_light_fsm.o SystemC::systemc)
