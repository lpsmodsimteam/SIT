cmake_minimum_required(VERSION 3.12)
project(socks CXX)

# increase buffer size (minimum is 22)
#add_definitions(-DBUFSIZE=50)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libsystemc.so ../common/unit.cpp blackboxes/ram_comp.cpp)

_sst_compile_link(libsystemc.so)
