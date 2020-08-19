cmake_minimum_required(VERSION 3.12)
project(sock CXX)

execute_process(
    COMMAND mv ${CMAKE_BINARY_DIR}/blackboxes/build.sbt ${CMAKE_BINARY_DIR}
)

file(
    APPEND ${CMAKE_BINARY_DIR}/build.sbt
    "sourcesInBase := false\n"
    "scalaSource in Compile := baseDirectory.value / \"../tests/chisel/sock\"\n"
    "unmanagedSourceDirectories in Compile += baseDirectory.value / \"/blackboxes\"\n"
    "unmanagedSourceDirectories in Compile += baseDirectory.value / \"../tests/chisel/common\"\n"
)

add_definitions(-DELEM_LIB="chisel${CMAKE_PROJECT_NAME}")
add_executable(libchisel${CMAKE_PROJECT_NAME}.so ../tests/unit.cpp ${CMAKE_BINARY_DIR}/blackboxes/ram_comp.cpp)
_sst_compile_link(libchisel${CMAKE_PROJECT_NAME}.so)
