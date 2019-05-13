# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST 8)](https://github.com/sstsimulator/sst-core) (with [SST Elements 8](https://github.com/sstsimulator/sst-elements)) and [SystemC 2.3](http://www.accellera.org/downloads/standards/systemc) (with [Transaction-Level Modeling Library (TLM 2)](https://www.doulos.com/knowhow/systemc/tlm2/))

For further details on the implementation, see the [system design document](/docs/sstscit.pdf).

## Table of Contents

- [Installation](#installation)
  - [CMake](#cmake)
  - [SystemC](#systemc)
  - [SST Core and Elements](#sst-core-and-elements)
  - [msgpack](#msgpack)
- [Requirements](#requirements)
  - [Languages and Compilers](#languages-and-compilers)
  - [Libraries](#libraries)
  - [Systems](#systems)
- [Usage](#usage)


## Installation

The toolkit itself is a collection of static header files that does not require any installation.
The requirements for the project are listed below.

### CMake

CMake is used to build the requirements as well as the toolkit. It is possible to build the project
with other build tools, but CMake makes project management extremely convenient.

```shell
CMAKE_URL="https://cmake.org/files/v${CMAKE_VER}/cmake-${CMAKE_VER}-Linux-x86_64.tar.gz"
curl -L ${CMAKE_URL} | tar xz -C deps
export PATH=deps/cmake-${CMAKE_VER}-Linux-x86_64/bin:${PATH}
cmake --version
```

### SystemC
```shell
SYSC_URL="https://www.accellera.org/images/downloads/standards/systemc/"
curl -L ${SYSC_URL}${SYSC_VER}.tar.gz | tar xz -C deps
mkdir -p deps/${SYSC_VER}/build
cd deps/${SYSC_VER}/build && cmake -DCMAKE_CXX_STANDARD=11 .. && make -j${JOBS} && sudo make install
```

### SST Core and Elements
```shell
SST_CORE_URL="https://github.com/sstsimulator/sst-core/releases/download/v${SST_VER}_Final/"
mkdir -p ~/.sst
touch ~/.sst/sstsimulator.conf
curl -L ${SST_CORE_URL}sstcore-${SST_VER}.tar.gz | tar xz -C deps
cd deps/sstcore-${SST_VER} && ./configure --disable-dependency-tracking && make -j${JOBS} all && sudo make install
cd -

SST_ELEM_URL="https://github.com/sstsimulator/sst-elements/releases/download/v${SST_VER}_Final/"
curl -L ${SST_ELEM_URL}sstelements-${SST_VER}.tar.gz | tar xz -C deps
mv deps/sst-elements* deps/sstelements-${SST_VER}
cd deps/sstelements-${SST_VER} && ./configure --disable-dependency-tracking && make -j${JOBS} all && sudo make install
```

### msgpack
```
./install
```

## Requirements

The following tables summarize the minimum versions of the languages and libraries used in the project.

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

Due to limitations with SST, this toolkit currently only works on UNIX systems. A list of supported Ubuntu Linux distributions can be found in `.travis.yml`.

## Usage

To establish interoperability between an SST model and a SystemC module, a black box SST model and SystemC driver must be generated. The black boxes establish the configurations required for the interprocess communication (IPC) between the SST and SystemC processes. The Python class `generate.BoilerPlate` can be used to generate the black boxes.

The following snippet demonstrates an example usage of the class to generate black box components for `demo`.

```python
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

generate.BoilerPlate(**DEMO_ARGS).generate_bbox()
```

The following snippets demonstrate an SST link transmitting a unidirectional signal from the SST
environment to the black box interface.

```c++
// demo.cpp

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
// blackboxes/demo_comp.cpp

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
