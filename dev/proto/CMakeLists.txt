cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
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
macro(_sst_compile_link _name)
    if (TARGET ${_name})
        target_compile_definitions(${_name} PRIVATE)
        target_compile_options(
                ${_name} PRIVATE
                -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/include
        )
        target_link_options(
                ${_name} PRIVATE
                -shared -fno-common -Wl,-undefined -Wl,dynamic_lookup
        )
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
add_executable(libsst_galois_lfsrSST.so sst_galois_lfsr.cpp)
_sst_compile_link(libsst_galois_lfsrSST.so)

add_executable(
        galois_lfsr.o
        mods/lfsr/galois_lfsr.hpp mods/lfsr/galois_lfsr_driver.cpp
)
target_link_libraries(galois_lfsr.o SystemC::systemc)
# -------------------- EXECUTABLES --------------------
