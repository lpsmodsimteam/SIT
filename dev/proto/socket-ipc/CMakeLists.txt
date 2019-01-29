cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
project(sstsysc CXX)

# SYSTEMC CONFIGS
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_executable(sysc_counter_module.o sysc_counter.cpp sysc_counter_tb.cpp)
target_link_libraries(sysc_counter_module.o SystemC::systemc)

# clang++  -std=c++1y -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/home/sabbir/local/sstcore-8.0.0/include -shared -fno-common -Wl,-undefined -Wl,dynamic_lookup -o libsst_counterSST.so sst_counter.cpp
add_compile_options(-std=c++1y -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/home/sabbir/local/sstcore-8.0.0/include -shared -fno-common -Wl,-undefined -Wl,dynamic_lookup)
add_executable(libsst_counterSST.so sst_counter.cpp)
