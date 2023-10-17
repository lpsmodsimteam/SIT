cmake_minimum_required(VERSION 3.12)
project(sock CXX)

execute_process(
    COMMAND cp ${CMAKE_BINARY_DIR}/gen/ram_driver.py .
)

add_definitions(-DELEM_LIB="pyrtl${CMAKE_PROJECT_NAME}")
add_executable(libpyrtl${CMAKE_PROJECT_NAME}.so ../../../tests/unit.cpp ${CMAKE_BINARY_DIR}/gen/ram_comp.cpp)
_sst_compile_link(libpyrtl${CMAKE_PROJECT_NAME}.so)
