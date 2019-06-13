# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) (with [SST Elements](https://github.com/sstsimulator/sst-elements)) and [SystemC](http://www.accellera.org/downloads/standards/systemc).

For further details on the implementation, see the [system design document](/docs/sstscit.pdf).

## Table of Contents

- [Usage](#usage)
  - [Tests](#tests)
- [Installation](#installation)
  - [CMake](#cmake)
  - [SystemC](#systemc)
  - [SST Core](#sst-core)
  - [MessagePack](#messagepack)
- [Requirements](#requirements)
  - [Languages and Compilers](#languages-and-compilers)
  - [Libraries](#libraries)
  - [Systems](#systems)


## Usage

To establish interoperability between an SST model and a SystemC module, a black box SST model and
SystemC driver must be generated. The black boxes establish the configurations required for the
interprocess communication (IPC) between the SST and SystemC processes. The Python class
`generate.BoilerPlate` can be used to generate the black boxes.

The following is the directory structure of an example component `demo` using CMake.

```bash
demo/                     # project directory
├── bbox.py               # boilerplate code generating script
├── blackboxes/           # black box interface
│   ├── demo_comp.cpp     # SystemC black box driver file
│   └── demo_driver.cpp   # SST black box component
├── demo.cpp              # SST parent component
├── demo_sysc.hpp         # SystemC module
├── build/                # CMake executables
├── CMakeLists.txt        # local CMakeLists for demo
├── tests/                # SST Python scripts
│   └── test.py           # sample SST Python scripts
├── Makefile              # local Makefile
├── .                     # other files
│   .
│   .
```

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/bbox.py

DEMO_ARGS = {
    "module" : "demo",
    "lib" : "demo_lib",
    "ipc" : "sock",
    "drvr_templ_path" : "/path/to/drvr_templ_path.hpp",
    "sst_model_templ_path" : "/path/to/model_templ_path.hpp",
    "desc" : "Demo SST Black Box Component",
    "link_desc" : {
        "link_desc0" : "Demo data_in",
        "link_desc1" : "Demo data_out",
    }
}

demo_obj = generate.BoilerPlate(**DEMO_ARGS)
demo_obj.set_ports((
    ("<bool>", "clock", "clock"),
    ("<bool>", "input1", "input"),
    ("<int>", "input2", "input"),
    ("<sc_uint<4> >", "data_out", "output"),
)).generate_bbox()
```

Running the script `python3 bbox.py` will generate the boilerplate code necessary to establish
interoperability between the black box components of `demo`.

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
project should be ready to interface the SystemC process.

### Tests

Simple unit tests can be ran by the Makefile in the `examples` directory.

To test the Python boilerplate code generator and the synchronization of SST and
SystemC processes via UNIX domain sockets, run `make test_min`.

To run all the unit tests, run `make test`. The unit tests essentially run the minimum unit tests
with all the supported IPC.

To run the intersection simulation, run `cd intersection && make install run`.

## Installation

The toolkit itself is a collection of static header files that does not require any installation.
The requirements for the project are listed below.

### CMake

CMake is used to build the requirements as well as the toolkit. It is possible to build the project
with other build tools, but CMake makes project management extremely convenient. Instructions on
installing CMake can be found [here](https://cmake.org/install/).

### SystemC

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

### MessagePack

MessagePack is required to maintain the complex data structures being passed via IPC. Installation
of this third party library is not necessary since inclusion of its header files are sufficient.
Running the script attached to the project will set up MessagePack for the toolkit.

```bash
# ${PROJ_HOME}/install

msgpack_url="https://github.com/msgpack/msgpack-c"
git clone --depth 1 -q ${msgpack_url} msgpack
mkdir -p sstscit/msgpack
mv msgpack/include/* sstscit/msgpack && rm -rf msgpack
```

## Requirements

The following tables summarize the minimum versions of the languages and libraries used in the
project.

### Languages and Compilers

|Language|Version|
|--------|-------|
|C++     |11     |
|Python  |3.6    |

|Compiler|Version|
|--------|-------|
|Clang   |6.0    |
|GCC     |6.0    |

### Libraries

|Requirement|Version|
|-----------|-------|
|CMake      |3.14   |
|msgpack    |3.1.1  |
|SST        |8.0.0  |
|SystemC    |2.3.3  |

### Systems

Due to limitations with SST, this toolkit currently only works on UNIX systems. A list of supported
Ubuntu Linux distributions can be found in `.travis.yml`.
