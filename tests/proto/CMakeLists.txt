cmake_minimum_required(VERSION 3.13)
project(sstscit CXX)
include(../../src/CMakeLists.txt)

# -------------------- EXECUTABLES --------------------
if (${UNIT_TEST})

    # -------------------- TESTING EXECUTABLES --------------------
    if (${DEMO_ZMQ})
        add_executable(libproto.so test-prototype.cpp blackboxes/galois_zmq.cpp blackboxes/fib_zmq.cpp)

        add_executable(galois_lfsr.o blackboxes/galois_zmq_driver.cpp)
        add_executable(fib_lfsr.o blackboxes/fib_zmq_driver.cpp)
    else ()
        add_executable(libproto.so test-prototype.cpp blackboxes/galois_sock.cpp blackboxes/fib_sock.cpp)

        add_executable(galois_lfsr.o blackboxes/galois_sock_driver.cpp)
        add_executable(fib_lfsr.o blackboxes/fib_sock_driver.cpp)
    endif ()
    # -------------------- TESTING EXECUTABLES --------------------

else ()

    # -------------------- DEMO EXECUTABLES --------------------
    if (${DEMO_ZMQ})
        add_executable(libproto.so prototype.cpp blackboxes/galois_zmq.cpp blackboxes/fib_zmq.cpp)

        add_executable(galois_lfsr.o blackboxes/galois_zmq_driver.cpp)
        add_executable(fib_lfsr.o blackboxes/fib_zmq_driver.cpp)
    else ()
        add_executable(libproto.so prototype.cpp blackboxes/galois_sock.cpp blackboxes/fib_sock.cpp)

        add_executable(galois_lfsr.o blackboxes/galois_sock_driver.cpp)
        add_executable(fib_lfsr.o blackboxes/fib_sock_driver.cpp)
    endif ()
    # -------------------- DEMO EXECUTABLES --------------------

endif ()

_sst_compile_link(libproto.so)
target_link_libraries(galois_lfsr.o SystemC::systemc)
target_link_libraries(fib_lfsr.o SystemC::systemc)
# -------------------- EXECUTABLES --------------------
