cmake_minimum_required(VERSION 3.12)
project(examples CXX)

set(CMAKE_CXX_STANDARD 11)
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
if (${ZMQ})

    find_package(cppzmq REQUIRED)
    if (${cppzmq_FOUND})
        message(STATUS "Using ZMQ for IPC")
        add_definitions(-DUSE_ZMQ)

        macro(add_executable _name)
            # invoke built-in add_executable
            _add_executable(${ARGV})
            if (TARGET ${_name})
                target_link_libraries(${_name} cppzmq)
            endif ()
        endmacro()
    endif ()

endif ()

get_filename_component(SRC_DIR ../../ssti ABSOLUTE)
include_directories(${SRC_DIR}/msgpack)

# -------------------- EXTERNAL LIBRARIES --------------------
