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

# increase buffer size (minimum is 5)
add_definitions(-DBUFSIZE=15)

add_executable(
    libplague.so
    strain.cpp
    systemc/blackboxes/randf_comp.cpp
    systemc/blackboxes/minf_comp.cpp
)
_sst_compile_link(libplague.so)

add_executable(randf.o systemc/blackboxes/randf_driver.cpp)
target_link_libraries(randf.o SystemC::systemc)

add_executable(minf.o systemc/blackboxes/minf_driver.cpp)
target_link_libraries(minf.o SystemC::systemc)
