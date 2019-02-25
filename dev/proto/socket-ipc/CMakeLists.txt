cmake_minimum_required(VERSION 3.13)
set(ENV{OMPI_CXX} clang++)
set(CMAKE_CXX_COMPILER /usr/bin/mpic++)
project(sstsysc CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(/usr/include/mpi/)
include_directories(/home/sabbir/local/sstcore-8.0.0/include)

# SYSTEMC CONFIGS
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

#add_executable(sysc_counter_module.o mods/simple-counter/counter.cpp mods/simple-counter/counter_driver.cpp)
#target_link_libraries(sysc_counter_module.o SystemC::systemc)
#add_executable(sysc_inverter_module.o mods/simple-counter/inverter.cpp mods/simple-counter/inverter_driver.cpp)
#target_link_libraries(sysc_inverter_module.o SystemC::systemc)
add_executable(galois_lfsr.o mods/lfsr/galois_lfsr.hpp mods/lfsr/galois_lfsr_driver.cpp)
target_link_libraries(galois_lfsr.o SystemC::systemc)
add_executable(lrs.o mods/lfsr/lrs.hpp mods/lfsr/lrs_driver.cpp)
target_link_libraries(lrs.o SystemC::systemc)

set(SST_OBJ "libsst_devSST.so")
set(SST_SRC "${PROJECT_SOURCE_DIR}/sst_dev.cpp")
set(SST_CONFIG /home/sabbir/local/sstcore-8.0.0/bin/sst-config)
execute_process(
    COMMAND ${SST_CONFIG} --ELEMENT_CXXFLAGS
    OUTPUT_VARIABLE SST_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${SST_CONFIG} --ELEMENT_LDFLAGS
    OUTPUT_VARIABLE SST_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(SST_FLAGS)
separate_arguments(SST_LDFLAGS)

add_custom_target(
    ${SST_OBJ}
    ALL
    DEPENDS ${SST_SRC}
    COMMAND ${CMAKE_CXX_COMPILER} ${SST_FLAGS} ${SST_LDFLAGS} -o ${SST_OBJ} ${SST_SRC}
)
