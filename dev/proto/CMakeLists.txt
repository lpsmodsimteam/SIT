cmake_minimum_required(VERSION 3.13)
project(sstscit CXX)
include(../../src/CMakeLists.txt)

# -------------------- EXECUTABLES --------------------
if (${DEMO_ZMQ})
    add_executable(libproto.so prototype.cpp blackboxes/galois_zmq.cpp blackboxes/fib_zmq.cpp)
    _sst_compile_link(libproto.so)

    add_executable(galois_lfsr.o blackboxes/galois_zmq_driver.cpp)
    target_link_libraries(galois_lfsr.o SystemC::systemc)
    add_executable(fib_lfsr.o blackboxes/fib_zmq_driver.cpp)
    target_link_libraries(fib_lfsr.o SystemC::systemc)
else ()
    add_executable(libproto.so prototype.cpp blackboxes/galois_sock.cpp blackboxes/fib_sock.cpp)
    _sst_compile_link(libproto.so)

    add_executable(galois_lfsr.o blackboxes/galois_sock_driver.cpp)
    target_link_libraries(galois_lfsr.o SystemC::systemc)
    add_executable(fib_lfsr.o blackboxes/fib_sock_driver.cpp)
    target_link_libraries(fib_lfsr.o SystemC::systemc)
endif ()
# -------------------- EXECUTABLES --------------------
