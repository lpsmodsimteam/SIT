# SST Interoperability Toolkit: Chisel

[Chisel](https://www.chisel-lang.org/) is a hardware design language that facilitates advanced circuit generation and design reuse for both ASIC and FPGA digital logic designs.

## Requirements

The following table summarizes the minimum versions of the languages and libraries supported:

|Requirement | Version|
|----------- | -------|
|Chisel      | 3.2    |
|iotesters   | 1.3    |

## Black Box Code Generation

### Usage
The `boilerplate.Chisel` class provides the following methods to generate the black box code:
- Constructor
```python
boilerplate.Chisel(
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
|`ipc`    |`str (options: "sock")`|method of IPC|
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
|`ports`|`tuple(tuple3(str))`|type-declared signals in the form `(<INTEGER BIT WIDTH>, <PORT NAME>, <PORT TYPE>)`<br>An integer bit width of `1` (a binary value of 0 or 1) represents the boolean type.<br>The current types of signals supported are `("input", "output")`.|

```python
boilerplate.Chisel.set_ports(ports)
```

- `generate_box()` - Provide a high-level interface to the user to generate both the components of the black box and dump them to their corresponding files
```python
boilerplate.Chisel.generate_bbox()
```

### Limitations
Since Scala wraps Java APIs for most of its functionality, it inherits a majority of the
disadvantages as well. Current versions of Java do not support interprocess communication and
require third party libraries for support. Due to such restrictions, the toolkit only supports IPC
via Unix domain sockets.

### Example

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/generate_bbox.py

import boilerplate

demo_obj = boilerplate.Chisel(
    module="demo",
    lib="demo_lib",
    ipc="sock",
    desc="Demo SST Black Box Component",
)
demo_obj.set_ports((
    ("2", "input1", "input"),
    ("4", "input2", "input"),
    ("4", "data_out", "output"),
))
demo_obj.generate_bbox()
```

The snippet generates `blackboxes/build.sbt`, `blackboxes/demo_driver.scala` and `blackboxes/demo_comp.cpp`.
