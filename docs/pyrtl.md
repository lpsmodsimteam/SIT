# SST Interoperability Toolkit: PyRTL

[PyRTL](https://ucsbarchlab.github.io/PyRTL/) is a Python library for register-transfer level design, simulation, tracing, and testing suitable for teaching and research.

## Installation

PyRTL can be installed via pip:
```bash
pip install pyrtl
```

## Requirements

The following table summarizes the minimum versions of the languages and libraries supported:

|Requirement|Version|
|-----------|-------|
|PyRTL      |0.8.7  |

## Black Box Code Generation

### Usage
The `boilerplate.PyRTL` class provides the following methods to generate the black box code:
- Constructor
```python
boilerplate.PyRTL(
    ipc=ipc, 
    module=module, 
    lib=lib, 
    module_dir="", 
    lib_dir="", 
    desc="",
    driver_template_path="", 
    component_template_path=""
)
```
|Parameter|Type|Description|
|---------|----|-----------|
|`ipc`    |`str (options: "sock", "zmq")`|method of IPC|
|`module` |`str`|SST element component and HDL module name|
|`lib`    |`str`|SST element library name|
|`module_dir`   | `str (default: "")` |directory of HDL module|
|`lib_dir`|`str (default: "")`|directory of SIT library|
|`desc`|`str (default: "")`|description of the SST model|
|`driver_template_path`|`str (default: "")`|path to the black box-driver boilerplate|
|`component_template_path`|`str (default: "")`|path to the black box-model boilerplate|

- `set_ports(ports)` - Assign ports to their corresponding member lists

|Parameter|Type|Description|
|---------|----|-----------|
|`ports`|`tuple(tuple3(str))`|type-declared signals in the form `(<PORT TYPE>, <PORT NAME>, <INTEGER BIT WIDTH>)`<br>An integer bit width of `1` (a binary value of 0 or 1) represents the boolean type.<br>The current types of signals supported are `("input", "output")`.<br>PyRTL does not require clock ports to be explicitly labeled due to Python's dynamically-typed system. Labeling an input as `clock` will be ignored. To specify a clock port, label it as `input` along with its data length.|

```python
boilerplate.PyRTL.set_ports(ports)
```

- `generate_box()` - Provide a high-level interface to the user to generate both the components of the black box and dump them to their corresponding files
```python
boilerplate.PyRTL.generate_bbox()
```

### Example

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/generate_bbox.py

import boilerplate

demo_obj = boilerplate.PyRTL(
    module="demo",
    lib="demo_lib",
    ipc="sock",
    desc="Demo SST Black Box Component",
)
demo_obj.set_ports((
    ("input", "input1", "2"),
    ("input", "input2", "4"),
    ("output", "data_out", "4"),
))
demo_obj.generate_bbox()
```

The snippet generates `blackboxes/demo_driver.py` and `blackboxes/demo_comp.cpp`.
