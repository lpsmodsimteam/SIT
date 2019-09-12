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

# increase buffer size (minimum is 5)
add_definitions(-DBUFSIZE=15)

add_executable(
    libintersection.so
    intersection.cpp
    car_generator.cpp
    systemc/traffic_light_systemc.cpp
    systemc/blackboxes/systemc_fsm_comp.cpp
    pyrtl/traffic_light_pyrtl.cpp
    pyrtl/blackboxes/pyrtl_fsm_comp.cpp
)

_sst_compile_link(libintersection.so)
add_executable(systemc_fsm.o systemc/blackboxes/systemc_fsm_driver.cpp)
target_link_libraries(systemc_fsm.o SystemC::systemc)
