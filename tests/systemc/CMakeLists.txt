cmake_minimum_required(VERSION 3.12)
project(unit CXX)
include(../CMakeLists.txt)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libproto.so unit.cpp blackboxes/galois_sock.cpp blackboxes/fib_sock.cpp)

add_executable(galois_lfsr.o blackboxes/galois_sock_driver.cpp)
add_executable(fib_lfsr.o blackboxes/fib_sock_driver.cpp)

_sst_compile_link(libproto.so)
target_link_libraries(galois_lfsr.o SystemC::systemc)
target_link_libraries(fib_lfsr.o SystemC::systemc)

add_executable(example test.cpp)
target_link_libraries(example gtest_main)
add_test(NAME example_test COMMAND example)
