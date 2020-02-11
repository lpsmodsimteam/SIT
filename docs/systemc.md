# SST Interoperability Toolkit: SystemC

[SystemC](https://www.accellera.org/community/systemc/about-systemc) is a set of C++ classes and macros which provide an event-driven simulation interface.

## Installation

SystemC can be built with CMake to allow more convenient inclusion in the project `CMakeLists.txt`.
The following snippet details instructions on installing SystemC with CMake in the project's home
directory.

```sh
# make directory to manage all dependencies
mkdir -p deps/systemc-${SYSC_VER}/build  # SYSC_VER is the version of SystemC
SYSC_URL="https://www.accellera.org/images/downloads/standards/systemc/"
curl -L ${SYSC_URL}${SYSC_VER}.tar.gz | tar xz -C deps
cd deps/systemc-${SYSC_VER}/build && cmake -DCMAKE_CXX_STANDARD=11 .. && make -j${JOBS} && sudo make install
```

## Requirements

The following table summarizes the minimum versions of the languages and libraries supported:

|Requirement|Version|
|-----------|-------|
|SystemC    |2.3.3  |

## Black Box Code Generation

### Usage
The `boilerplate.SystemC` class provides the following methods to generate the black box code:
- Constructor
```python
boilerplate.SystemC(
    ipc=ipc, 
    module=module,
    lib=lib,
    width_macros=None,
    module_dir="",
    lib_dir="", 
    desc="",
    driver_template_path="",
    component_template_path=""
)
```
|Parameter|Type|Description|
|---------|----|-----------|
|`ipc`    |`str (options: "sock")`|method of IPC|
|`module` |`str`|SST element component and HDL module name|
|`lib`    |`str`|SST element library name|
|`width_macros`|`dict(str:[str\|int]) (default: dict(None:None))`|mapping of signal width macros to their integer values|
|`module_dir`   | `str (default: "")` |directory of HDL module|
|`lib_dir`|`str (default: "")`|directory of SIT library|
|`desc`|`str (default: "")`|description of the SST model|
|`driver_template_path`|`str (default: "")`|path to the black box-driver boilerplate|
|`component_template_path`|`str (default: "")`|path to the black box-model boilerplate|

- `set_ports(ports)` - Assign ports to their corresponding member lists

|Parameter|Type|Description|
|---------|----|-----------|
|`ports`|`tuple(tuple3(str))`|type-declared signals in the form `(<DATA TYPE>, <PORT NAME>, <PORT TYPE>)`<br>`<DATA TYPE>` are represented as C++ style data types where the integer width must also be provided as necessary.<br>The current types of signals supported are `("input", "output", "clock", "inout")`.|

```python
boilerplate.SystemC.set_ports(ports)
```

- `generate_box()` - Provide a high-level interface to the user to generate both the components of the black box and dump them to their corresponding files
```python
boilerplate.SystemC.generate_bbox()
```

### Example

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/generate_bbox.py

import boilerplate

demo_obj = boilerplate.SystemC(
    module="demo",
    lib="demo_lib",
    ipc="sock",
    desc="Demo SST Black Box Component",
    width_macros={
        "DATA_WIDTH": 4,
    },
)
demo_obj.set_ports((
    ("clock", "clock", "<bool>"),
    ("input", "input1", "<bool>"),
    ("input", "input2", "<int>"),
    ("output", "data_out", "<sc_uint<DATA_WIDTH> >"),
))
demo_obj.generate_bbox()
```

The snippet generates `blackboxes/demo_driver.cpp` and `blackboxes/demo_comp.cpp`.
