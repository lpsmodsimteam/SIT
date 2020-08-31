# SST Interoperability Toolkit
![gcc builds](https://github.com/sabbirahm3d/SIT/workflows/gcc/badge.svg)
![clang builds](https://github.com/sabbirahm3d/SIT/workflows/clang/badge.svg)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) and external hardware description languages (HDL).

___Note__: For the sake of simplicity and consistency in the nomenclature, the languages, toolkits or libraries in the following categories will be simply labeled as HDL:_
- _hardware description languages (SystemVerilog, Verilog, VHDL, etc.)_
- _hardware level modeling languages (Chisel, PyRTL, SystemC, etc.)_

For interoperability with supported HDLs, see their corresponding README:
- [Chisel](/docs/chisel.md)
- [PyRTL](/docs/pyrtl.md)
- [SystemC](/docs/systemc.md)
- [Verilog](/docs/verilog.md)

## Table of Contents

- [Usage](#usage)
  - [Black Box Code Generation](#black-box-code-generation)
  - [Architecture](#architecture)
  - [Tests](#tests)
- [Installation](#installation)
  - [boilerplate](#boilerplate)
  - [CMake](#cmake)
  - [SST Core](#sst-core)
- [Requirements](#requirements)
  - [Languages, Compilers and Libraries](#languages,-compilers-and-libraries)
  - [Systems](#systems)


## Usage

### Black Box Code Generation

To establish interoperability between an SST model and an HDL module, a black box SST component and
HDL driver must be generated. The black box establishes the configurations required for the
interprocess communication (IPC) between the SST and the external HDL processes.

The Python module `boilerplate` can be used to generate the black boxes.

### Architecture

The following is the directory structure of an example component `demo` using CMake.

```bash
demo/                     # project directory
├── generate_bbox.py      # boilerplate code generating script
├── blackboxes/           # black box interface
│   ├── demo_comp.cpp     # SST black box component
│   └── demo_driver.hdl   # HDL black box driver
├── demo.cpp              # SST parent component
├── demo.hdl              # HDL module
├── build/                # CMake executables
├── CMakeLists.txt        # local CMakeLists for demo
├── tests/                # SST Python scripts
│   └── run.py            # sample SST Python scripts
├── .                     # other files
│   .
│   .
```

The following snippets demonstrate an SST link transmitting a unidirectional signal from the SST
environment to the black box interface.

```c++
// demo/demo.cpp

// register a string event in the class declaration
SST_ELI_DOCUMENT_PORTS(
    { "demo_din", "Demo model data in", { "sst.Interfaces.StringEvent" }},
    ...
)

// initialize the link in the class declaration
SST::Link *demo_din;

// set up the SST link in the constructor
demo_din = configureLink("demo_din");

// trigger the event in the clocked function
demo_din->send(new SST::Interfaces::StringEvent(...));
```


```c++
// demo/blackboxes/demo_comp.cpp

// register the same string event in the class declaration
SST_ELI_DOCUMENT_PORTS(
    { "demo_din", "Demo model data in", { "sst.Interfaces.StringEvent" }},
    ...
)

// initialize the same link in the class declaration
SST::Link *demo_din;

// set up the SST link in the constructor with an event handler
demo_din = configureLink(
    "demo_din",
    new SST::Event::Handler<demo>(this, &demo::handle_event)
);

// receive and parse the event in the event handler
void demo::handle_event(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::string _data_in = se->getString();
        ...
    }
    delete ev;
}
```

Once the SST Links are established between the black box interface and the parent component, the
project should be ready to interface the HDL process.

### Tests

Simple unit tests can be ran by the Makefile in the `tests` directory.

## Installation

The toolkit itself is a collection of static header files and can be installed as an interface using CMake.

```shell
mkdir build
cd build
cmake ..
sudo make install
```

The requirements for the project are listed below.

### boilerplate

The `boilerplate` module can be installed locally by running `pip`:
```shell
mkdir -p $(sst-config --prefix)/include/sst/sit

cd $(sst-config --prefix)/include/sst/sit
pip install .
```

See [Black Box Code Generation](#black-box-code-generation) for its purposes.

### CMake

CMake is used to build the requirements as well as the toolkit. It is possible to build the project
with other build tools, but CMake makes project management extremely convenient. Instructions on
installing CMake can be found [here](https://cmake.org/install/).

### SST Core

The following snippet details instructions on installing SST Core in the project's home directory.

```sh
# make directory to manage all dependencies
mkdir -p deps
# initialize empty file for SST configurations
mkdir -p ~/.sst && touch ~/.sst/sstsimulator.conf

# install OpenMPI and its dependencies
sudo apt install libhwloc-dev libopenmpi-dev libtool-bin

curl -L ${SST_CORE_URL} | tar xz -C deps
cd deps/sstcore-* && ./configure && make -j all && sudo make install
```

## Requirements

The following tables summarize the minimum versions of the languages and libraries used in the
project.

### Languages, Compilers and Libraries

|Language|Version|
|--------|-------|
|C++     |11     |
|Python  |3.6    |

#### C++ Compilers

|Version|7.0|8.0|9.0|10.0|
|-------|:-:|:-:|:-:|:--:|
|Clang  |✔️ |✔️ |✔️ |✔️  |
|GCC    |✔️ |✔️ |✔️ |✔️  |

|Library|Version|
|-------|-------|
|CMake  |3.14   |
|SST    |10.0.0 |

### Systems

Due to limitations with SST, this toolkit currently only works on UNIX systems. A list of supported
Ubuntu Linux distributions can be found in `.github/workflows/clang.yml` and `.github/workflows/gcc.yml`.
