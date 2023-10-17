# SST Interoperability Toolkit

A toolkit to provide support for interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) and external hardware description languages (HDL).

## Table of Contents

- [Requirements](#requirements)
  - [Supported HDLs](#supported-hdls)
    - [PyRTL](#pyrtl)
    - [SystemC](#systemc)
    - [Verilog](#verilog)
- [Installation](#installation)
- [Usage](#usage)
  - [Boilerplate Code Generation](#black-box-code-generation)
    - [Library Parameters](#library-parameters)
    - [Configuration Parameters](#configuration-parameters)
    - [Port Parameters](#port-parameters)

## Requirements

The current version of SIT requires:

- SST Core 10.0.0+
- Python 3.6+
- CMake 3.18+
- GNU Make 3+

Additionally, supported HDLs have their own sets of requirements.

### Supported HDLs

___Note__: For the sake of consistency, the languages, toolkits or libraries in the following categories will be simply labeled as HDL:_
- _hardware description languages (SystemVerilog, Verilog, VHDL, etc.)_
- _hardware level modeling languages (PyRTL, SystemC, etc.)_

#### PyRTL

[PyRTL](https://ucsbarchlab.github.io/PyRTL/) provides a collection of classes for pythonic register-transfer level design, simulation, tracing, and testing suitable for teaching and research.

The library can be installed via pip: `pip install pyrtl`.

#### SystemC

[SystemC](https://systemc.org/overview/systemc/) is a C++ library and a set of tools for hardware and system-level modeling and simulation.

The library can be installed by downloading the source and [following their instructions](https://github.com/accellera-official/systemc/blob/master/INSTALL.md).

#### Verilog

Verilog modules have the following requirements:

- [cocotb](https://github.com/cocotb/cocotb), a coroutine based cosimulation library for writing VHDL and Verilog testbenches in Python.
- An HDL simulator (such as [Icarus Verilog](https://docs.cocotb.org/en/stable/simulator_support.html#icarus-verilog),
[Verilator](https://docs.cocotb.org/en/stable/simulator_support.html#verilator),
[GHDL](https://docs.cocotb.org/en/stable/simulator_support.html#ghdl) or
[other simulator](https://docs.cocotb.org/en/stable/simulator_support.html))

## Installation

The toolkit can be installed using [pip](https://pip.pypa.io/en/stable/getting-started/):

`$ pip install sst-it`

The Python package ships with the header library required to interoperate with SST. To install the library, run the console script:

`$ sit --install`

### Development

To set up the development version, clone the repository and create a virtual environment.

- Install the toolkit using pip: `pip install .`
- Create a directory to store the binaries and change to it: `mkdir .build && cd .build`
- Run the library CMakeFiles.txt: `cmake ../src/sit/cpp/`

## Usage

### Boilerplate Code Generation

To establish interoperability between an SST model and an HDL module, a boilerplate SST component and HDL driver must be generated. The boilerplate layer establishes the configurations required for the interprocess communication (IPC) between the SST and the external HDL processes.

The boilerplate code generation library accepts arguments in a JSON format.

#### Library Parameters

|Parameter|Description|
|---------|-----------|
|`hdl`|Name of the hardware description language. The string value must correspond to a supported HDL. Valid options are `{"pyrtl"\|"systemc"\|"verilog"}`|
|`config`|High level configuration parameters, including library names and locations of module files|
|`ports`|Description of the ports. Ports can be of type: `"input", "output", "inout"`|

#### Configuration Parameters

|Parameter|Description|Type|
|---------|-----------|----|
|`config.ipc`|IPC method for the boilerplate layer|`{"sock"\|"zmq"}`|
|`config.module_name`|Name of the module, i.e. SST Component name and HDL module name|`str`|
|`config.lib`|Name of the library, i.e. SST Component library name|`str`|
|`config.desc`|Description of the module|`str`|
|`config.lib_dir`|Location of the library|`str`|
|`config.module_dir`|Location of the module|`str`|

#### Port Parameters

|Parameter|Description|Type|
|---------|-----------|----|
|`ports.input`|Array of input port objects|`list[dict[str,str\|int]]`|
|`ports.output`|Array of output port objects|`list[dict[str,str\|int]]`|
|`ports.inout`|Array of inout port objects|`list[dict[str,str\|int]]`|
|`ports.{"input"\|"output"\|"inout"}.name`|Name of port|`str`|
|`ports.{"input"\|"output"\|"inout"}.type`|Data type of port|`str`|
|`ports.{"input"\|"output"\|"inout"}.len`|Buffer length of port|`int`|

The following is an example configuration:
```python
config = {
    "hdl": "verilog",
    "config": {
        "module_name": "ram",
        "lib": "verilogsock",
        "desc": "Demonstration of a Verilog hardware simulation in SST",
        "module_dir": "../tests/verilog/",
    },
    "ports": {
        "input": [
            {"name": "address", "type": "bit", "len": 8},
            {"name": "cs", "type": "int", "len": 1},
            {"name": "we", "type": "int", "len": 1},
            {"name": "oe", "type": "int", "len": 1},
            {"name": "data_in", "type": "bit", "len": 8},
        ],
        "output": [{"name": "data_out", "type": "bit", "len": 8}],
    }
}
```

The configuration can be passed into the class constructor to generate the boilerplate layer.

```python
from sit import SIT

sit_obj = SIT(config)
sit_obj.generate_boilerplate()
```

The boilerplate layer code will be generated and saved to the directory `./gen`.
