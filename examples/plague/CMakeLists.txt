cmake_minimum_required(VERSION 3.12)
project(plague CXX)
include(../../tests/CMakeLists.txt)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

add_executable(
    libplague.so
    plague.cpp
    chisel/blackboxes/ram_comp.cpp
    systemc/blackboxes/minf_comp.cpp
    systemc/blackboxes/randf_comp.cpp
    systemc/blackboxes/rng_comp.cpp
    # systemc/blackboxes/sc_abs_comp.cpp
    systemc/blackboxes/sc_ceil_comp.cpp
    systemc/blackboxes/sc_exp_comp.cpp
)
_sst_compile_link(libplague.so)

add_executable(minf.o systemc/blackboxes/minf_driver.cpp)
target_link_libraries(minf.o SystemC::systemc)

add_executable(randf.o systemc/blackboxes/randf_driver.cpp)
target_link_libraries(randf.o SystemC::systemc)

add_executable(rng.o systemc/blackboxes/rng_driver.cpp)
target_link_libraries(rng.o SystemC::systemc)

# add_executable(sc_abs.o systemc/blackboxes/sc_abs_driver.cpp)
# target_link_libraries(sc_abs.o SystemC::systemc)

add_executable(sc_ceil.o systemc/blackboxes/sc_ceil_driver.cpp)
target_link_libraries(sc_ceil.o SystemC::systemc)

add_executable(sc_exp.o systemc/blackboxes/sc_exp_driver.cpp)
target_link_libraries(sc_exp.o SystemC::systemc)
