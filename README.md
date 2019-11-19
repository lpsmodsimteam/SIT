# SST Interoperable
![](https://github.com/sabbirahm3d/ssti/workflows/Continuous%20Integration/badge.svg)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) and external hardware description languages (HDL).

For interoperability with specific hardware description/ hardware-level modeling languages, see their corresponding README:
- [PyRTL](/docs/pyrtl.md)
- [SystemC](/docs/systemc.md)

## Table of Contents

- [Usage](#usage)
  - [Black Box Code Generation](#black-box-code-generation)
  - [Tests](#tests)
- [Installation](#installation)
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
├── Makefile              # local Makefile
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
project should be ready to interface the PyRTL process.

### Tests

Simple unit tests can be ran by the Makefile in the `tests` directory.

## Installation

The toolkit itself is a collection of static header files that does not require any installation.
The requirements for the project are listed below.

### CMake

CMake is used to build the requirements as well as the toolkit. It is possible to build the project
with other build tools, but CMake makes project management extremely convenient. Instructions on
installing CMake can be found [here](https://cmake.org/install/).

### SST Core

The following snippet details instructions on installing SST Core in the project's home directory.

```bash
# make directory to manage all dependencies
mkdir -p deps
# initialize empty file for SST configurations
mkdir -p ~/.sst && touch ~/.sst/sstsimulator.conf

# install OpenMPI and its dependencies
sudo apt install libhwloc-dev libopenmpi-dev libtool-bin

SST_CORE_URL="https://github.com/sstsimulator/sst-core/releases/download/v${SST_VER}_Final/"
curl -L ${SST_CORE_URL}sstcore-${SST_VER}.tar.gz | tar xz -C deps  # SST_VER is the version of SST
cd deps/sstcore-${SST_VER} && ./configure && make -j${JOBS} all && sudo make install
```

## Requirements

The following tables summarize the minimum versions of the languages and libraries used in the
project.

### Languages, Compilers and Libraries

|Language|Version|
|--------|-------|
|C++     |11     |
|Python  |3.6    |

|Compiler|Version|
|--------|-------|
|Clang   |6.0    |
|GCC     |6.0    |

|Library|Version|
|-------|-------|
|CMake  |3.14   |
|SST    |8.0.0  |

### Systems

Due to limitations with SST, this toolkit currently only works on UNIX systems. A list of supported
Ubuntu Linux distributions can be found in `.travis.yml`.
