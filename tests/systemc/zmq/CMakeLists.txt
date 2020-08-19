cmake_minimum_required(VERSION 3.12)
project(zmq CXX)

add_executable(ram.o ${CMAKE_BINARY_DIR}/blackboxes/ram_driver.cpp)
target_link_libraries(ram.o SystemC::systemc)

add_definitions(-DELEM_LIB="systemc${CMAKE_PROJECT_NAME}")
add_executable(libsystemc${CMAKE_PROJECT_NAME}.so ../tests/unit.cpp ${CMAKE_BINARY_DIR}/blackboxes/ram_comp.cpp)
_sst_compile_link(libsystemc${CMAKE_PROJECT_NAME}.so)
