# SST Interoperability Toolkit
<!-- ![gcc builds](https://github.com/sabbirahm3d/SIT/workflows/gcc/badge.svg)
![clang builds](https://github.com/sabbirahm3d/SIT/workflows/clang/badge.svg)
 -->

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) and external hardware description languages (HDL).

## Table of Contents

- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
  - [Black Box Code Generation](#black-box-code-generation)
- [Supported HDLs](#supported-hdls)
  - [PyRTL](#pyrtl)
  - [SystemC](#systemc)
  - [Verilog](#verilog)

## Requirements

The current stable version of SIT requires:

- SST Core 10.0.0+
- Python 3.6+
- CMake 3.18+
- GNU Make 3+

Additionally, supported HDLs have their own sets of requirements. See 

## Installation

The toolkit can be installed using [pip](https://pip.pypa.io/en/stable/getting-started/):

`$ pip install sst-it`

The Python package ships with the header library required to interoperate with SST. To install the library, run the console script:

`$ sit --install`

### Development

To set up the development version, clone the repository and create a virtual environment. Install the toolkit using pip:

`$ pip install .`

Run CMake on the CMakeLists.txt in `/src/sit/cpp/`.

## Usage

### Black Box Code Generation

To establish interoperability between an SST model and an HDL module, a black box SST component and
HDL driver must be generated. The black box establishes the configurations required for the
interprocess communication (IPC) between the SST and the external HDL processes.

## Supported HDLs

___Note__: For the sake of simplicity and consistency in the nomenclature, the languages, toolkits or libraries in the following categories will be simply labeled as HDL:_
- _hardware description languages (SystemVerilog, Verilog, VHDL, etc.)_
- _hardware level modeling languages (PyRTL, SystemC, etc.)_

### PyRTL

While PyRTL blackboxes can be generated, they are currently in the alpha stages of development and should not be used in production. Usage of the classes are discouraged, but they can be accessed via:

```python
from sit import SITPyRTL
```

### SystemC

While SystemC blackboxes can be generated, they are currently in the alpha stages of development and should not be used in production. Usage of the classes are discouraged, but they can be accessed via:

```python
from sit import SITSystemC
```

### Verilog

Verilog modules have the following requirements:

- cocotb
- An HDL simulator (such as [Icarus Verilog](https://docs.cocotb.org/en/stable/simulator_support.html#icarus-verilog),
[Verilator](https://docs.cocotb.org/en/stable/simulator_support.html#verilator),
[GHDL](https://docs.cocotb.org/en/stable/simulator_support.html#ghdl) or
[other simulator](https://docs.cocotb.org/en/stable/simulator_support.html))

```python
from sit import SITVerilog
```

