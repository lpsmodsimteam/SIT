cmake_minimum_required(VERSION 3.12)
project(tests CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# -------------------- SST CONFIGS --------------------
find_package(SIT CONFIG)
if (${SIT_FOUND})

    set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

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
            execute_process(COMMAND ${SST_REGISTER} ${_name} ${_name}_LIBDIR=${CMAKE_BINARY_DIR})
        endif ()
    endmacro()

    # -------------------- SST CONFIGS --------------------

    macro(_sst_run _hdl _ipc)
        add_custom_target(
            ${_hdl}_${_ipc}
            COMMAND ${SST_BIN} ${TESTS_DIR}/${_hdl}/${_ipc}/run.py
            COMMAND python ${TESTS_DIR}/read_mem.py memory_dump.txt
        )
        execute_process(
            COMMAND python ${TESTS_DIR}/${_hdl}/generate_bbox.py ${_ipc}
        )
        include(${TESTS_DIR}/${_hdl}/${_ipc}/CMakeLists.txt)
    endmacro()

    if (NOT IPC)
        set(IPC "sock")
    endif()

    if ("${HDL}" STREQUAL "SYSTEMC")

        include(${TESTS_DIR}/systemc/CMakeLists.txt)
        _sst_run(systemc ${IPC})

    elseif ("${HDL}" STREQUAL "PYRTL")

        _sst_run(pyrtl ${IPC})

    elseif ("${HDL}" STREQUAL "VERILOG")

        _sst_run(verilog ${IPC})

    elseif ("${HDL}" STREQUAL "CHISEL")

        _sst_run(chisel sock)

    endif()

    unset(IPC CACHE)

endif()
