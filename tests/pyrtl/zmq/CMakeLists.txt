cmake_minimum_required(VERSION 3.12)
project(zmq CXX)

find_package(cppzmq REQUIRED)
if (${cppzmq_FOUND})

    message(STATUS "Using ZMQ for IPC")
    add_definitions(-DUSE_ZMQ)

    macro(add_executable _name)
        # invoke built-in add_executable
        _add_executable(${ARGV})
        if (TARGET ${_name})
            target_link_libraries(${_name} cppzmq)
        endif ()
    endmacro()
endif ()

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(libtests.so ../../testcases/unit.cpp blackboxes/ram_comp.cpp)
_sst_compile_link(libtests.so)
