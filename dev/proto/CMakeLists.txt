cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
project(sstsysc CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------- SYSTEMC CONFIGS --------------------
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
# -------------------- SYSTEMC CONFIGS --------------------


# -------------------- SST CONFIGS --------------------
add_executable(libsst_galois_lfsrSST.so sst_galois_lfsr.cpp)
target_compile_definitions(libsst_galois_lfsrSST.so PRIVATE)
target_compile_options(
        libsst_galois_lfsrSST.so PRIVATE
        -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/include
)
target_link_options(
        libsst_galois_lfsrSST.so PRIVATE
        -shared -fno-common -Wl,-undefined -Wl,dynamic_lookup
)
target_link_libraries(libsst_galois_lfsrSST.so cppzmq)

# -------------------- SST CONFIGS --------------------

# -------------------- EXTERNAL LIBRARIES --------------------
find_package(cppzmq)
find_package(msgpack)
# -------------------- EXTERNAL LIBRARIES --------------------

add_executable(
        galois_lfsr.o
        mods/lfsr/galois_lfsr.hpp mods/lfsr/galois_lfsr_driver.cpp
)
target_link_libraries(galois_lfsr.o SystemC::systemc cppzmq)
