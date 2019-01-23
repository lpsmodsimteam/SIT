cmake_minimum_required(VERSION 3.10)
project (sysc_counter_module.o CXX)
set (CMAKE_PREFIX_PATH /opt/systemc)
message(STATUS "start running cmake...")

find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_executable(sysc_counter_module.o sysc_counter.cpp sysc_counter_tb.cpp)
target_link_libraries(sysc_counter_module.o SystemC::systemc)
