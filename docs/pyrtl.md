# SST Interoperable: SystemC

## Table of Contents

- [Usage](#usage)
  - [Code Generation](#code-generation)
  - [Tests](#tests)
- [Installation](#installation)
- [Requirements](#requirements)
  - [Languages and Compilers](#languages-and-compilers)
  - [Libraries](#libraries)
  - [Systems](#systems)


## Usage

### Code Generation

To establish interoperability between an SST model and a SystemC module, a black box SST model and
SystemC driver must be generated. The black boxes establish the configurations required for the
interprocess communication (IPC) between the SST and SystemC processes. The Python class
`boilerplate.BoilerPlate` can be used to generate the black boxes.

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

Running the script `python3 generate_bbox.py` will generate the boilerplate code necessary to
establish interoperability between the black box components of `demo`.

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

Simple unit tests can be ran by the Makefile in the `tests` directory.

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
|SST        |8.0.0  |
|SystemC    |2.3.3  |

### Systems

Due to limitations with SST, this toolkit currently only works on UNIX systems. A list of supported
Ubuntu Linux distributions can be found in `.travis.yml`.
