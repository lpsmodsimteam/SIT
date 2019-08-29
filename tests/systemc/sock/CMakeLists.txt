cmake_minimum_required(VERSION 3.12)
project(sock CXX)

# increase buffer size (minimum is 22)
add_definitions(-DBUFSIZE=50)

add_executable(ram.o blackboxes/ram_driver.cpp)
target_link_libraries(ram.o SystemC::systemc)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libtests.so ../../testcases/unit.cpp blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
