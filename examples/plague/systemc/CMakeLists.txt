cmake_minimum_required(VERSION 3.12)
project(systemc CXX)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

add_executable(test_driver.o test_driver.cpp)
target_link_libraries(test_driver.o SystemC::systemc)
