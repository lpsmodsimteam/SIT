# SST Interoperable: SystemC

## Black Box Code Generation

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/generate_bbox.py

import boilerplate

DEMO_ARGS = {
    "module" : "demo",
    "lib" : "demo_lib",
    "ipc" : "sock",
    "desc" : "Demo SST Black Box Component",
}

demo_obj = boilerplate.SystemC(**DEMO_ARGS)
demo_obj.set_ports((
    ("<bool>", "clock", "clock"),
    ("<bool>", "input1", "input"),
    ("<int>", "input2", "input"),
    ("<sc_uint<4> >", "data_out", "output"),
)).generate_bbox()
```

Running the script `python generate_bbox.py` will generate the boilerplate code necessary to
establish interoperability between the black box components of `demo`.

## Installation

SystemC can be built with CMake to allow more convenient inclusion in the project `CMakeLists.txt`.
The following snippet details instructions on installing SystemC with CMake in the project's home
directory.

```bash
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
