cmake_minimum_required(VERSION 3.13)
project(sstscit CXX)
include(../../src/CMakeLists.txt)

# -------------------- EXECUTABLES --------------------
 add_executable(libproto.so sst_dev.cpp blackboxes/galois_lfsr.cpp blackboxes/fib_lfsr.cpp)
 _sst_compile_link(libproto.so)

 add_executable(galois_lfsr.o blackboxes/galois_lfsr_driver.cpp)
 target_link_libraries(galois_lfsr.o SystemC::systemc)
 add_executable(fib_lfsr.o blackboxes/fib_lfsr_driver.cpp)
 target_link_libraries(fib_lfsr.o SystemC::systemc)
#add_executable(libsocktest.so scratch/socktest.cpp)
#_sst_compile_link(libsocktest.so)
#
#add_executable(galois_lfsr.o scratch/galois_lfsr_driver.cpp)
#target_link_libraries(galois_lfsr.o SystemC::systemc)
# -------------------- EXECUTABLES --------------------
