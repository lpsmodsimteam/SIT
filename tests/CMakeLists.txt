cmake_minimum_required(VERSION 3.12)
project(tests CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

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

execute_process(
    COMMAND which sst-register
    OUTPUT_VARIABLE SST_REGISTER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND which sst
    OUTPUT_VARIABLE SST_BIN
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

macro(_sst_compile_link _name)
    if (TARGET ${_name})
        target_compile_definitions(${_name} PRIVATE)
        target_compile_options(${_name} PRIVATE ${SST_FLAGS})
        target_link_options(${_name} PRIVATE ${SST_LDFLAGS})
        # execute_process(COMMAND ${SST_REGISTER} ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_LIBDIR=${CMAKE_CURRENT_SOURCE_DIR}/build)
    endif ()
endmacro()

add_custom_target(
    uninstall
    # COMMAND ${SST_REGISTER} -u ${CMAKE_PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} --build . --target clean --config $<CONFIG>
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/blackboxes
    COMMAND rm memory_dump.txt
    DEPENDS clean
)

# -------------------- SST CONFIGS --------------------

find_package(SIT CONFIG REQUIRED)

include(systemc/CMakeLists.txt)

macro(_generate _hdl _ipc)
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/blackboxes")
        message(STATUS "making")
        execute_process(
            COMMAND python ${PROJECT_SOURCE_DIR}/${_hdl}/generate_bbox.py ${_ipc}
        )
    endif()
    add_subdirectory(${_hdl}/${_ipc})
endmacro()

macro(_sst_run _hdl _ipc)
    add_custom_target(run_${_hdl}_${_ipc}
        COMMAND ${SST_BIN} ${PROJECT_SOURCE_DIR}/${_hdl}/${_ipc}/run.py
        COMMAND python ${PROJECT_SOURCE_DIR}/read_mem.py memory_dump.txt
    )
    add_custom_target(
        test_sst DEPENDS run_${_hdl}_${_ipc}
    )
endmacro()

_sst_run(systemc sock)
_generate(systemc sock)
# _sst_run(systemc zmq)
# _generate(systemc zmq)

_sst_run(pyrtl sock)
_generate(pyrtl sock)
# _sst_run(pyrtl zmq)
# _generate(pyrtl zmq)

_sst_run(verilog sock)
_generate(verilog sock)
# _sst_run(verilog zmq)
# _generate(verilog zmq)
