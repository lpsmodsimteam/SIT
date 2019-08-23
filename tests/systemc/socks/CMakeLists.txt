cmake_minimum_required(VERSION 3.12)
project(socks CXX)

# increase buffer size (minimum is 22)
add_definitions(-DBUFSIZE=50)

add_executable(ram.o blackboxes/ram_driver.cpp)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libsystemc.so ../unit.cpp blackboxes/ram_comp.cpp)

_sst_compile_link(libsystemc.so)
target_link_libraries(ram.o SystemC::systemc)
