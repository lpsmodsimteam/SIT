cmake_minimum_required(VERSION 3.13)
project(sstscit CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------

# -------------------- SST CONFIGS --------------------
execute_process(
    COMMAND which sst-config
    OUTPUT_VARIABLE SST_CONFIG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
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

macro(_sst_compile_link _name)
    if (TARGET ${_name})
        target_compile_definitions(${_name} PRIVATE)
        target_compile_options(${_name} PRIVATE ${SST_FLAGS})
        target_link_options(${_name} PRIVATE ${SST_LDFLAGS})
    endif ()
endmacro()
# -------------------- SST CONFIGS --------------------

# -------------------- EXTERNAL LIBRARIES --------------------
find_package(cppzmq)
find_package(msgpack)

macro(add_executable _name)
    # invoke built-in add_executable
    _add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} cppzmq)
    endif ()
endmacro()
# -------------------- EXTERNAL LIBRARIES --------------------

# -------------------- EXECUTABLES --------------------
add_executable(libproto.so sst_dev.cpp sst_galois_lfsr.cpp sst_fib_lfsr.cpp)
_sst_compile_link(libproto.so)

add_executable(galois_lfsr.o mods/lfsr/galois_lfsr_driver.cpp)
target_link_libraries(galois_lfsr.o SystemC::systemc)
add_executable(fib_lfsr.o mods/lfsr/fib_lfsr_driver.cpp)
target_link_libraries(fib_lfsr.o SystemC::systemc)
# -------------------- EXECUTABLES --------------------
