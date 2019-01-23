cmake_minimum_required(VERSION 3.10)
project (sstsysc)

find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_executable(sysc_counter_module.o sysc_counter_tb.cpp)
target_link_libraries(sstsysc SystemC::systemc)
