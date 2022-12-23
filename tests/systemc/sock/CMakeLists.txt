cmake_minimum_required(VERSION 3.12)
project(sock CXX)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

add_executable(ram.o ${CMAKE_BINARY_DIR}/gen/ram_driver.cpp)
target_link_libraries(ram.o SystemC::systemc)

add_definitions(-DELEM_LIB="systemc${CMAKE_PROJECT_NAME}")
add_executable(libsystemc${CMAKE_PROJECT_NAME}.so ../../../tests/unit.cpp ${CMAKE_BINARY_DIR}/gen/ram_comp.cpp)
_sst_compile_link(libsystemc${CMAKE_PROJECT_NAME}.so)
