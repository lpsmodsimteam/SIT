cmake_minimum_required(VERSION 3.12)
project(sock CXX)

# decrease buffer size
add_definitions(-DBUFSIZE=5)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libtests.so ../../testcases/unit.cpp blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
