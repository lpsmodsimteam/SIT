# SystemC-SST Interoperability Library
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/systemc-sst?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/systemc-sst&amp;utm_campaign=Badge_Grade)

A library to provide interoperability between
[Structural Simulation Toolkit (SST) 8.0.0](https://github.com/sstsimulator/sst-core)
(with [SST Elements 8.0.0](https://github.com/sstsimulator/sst-elements)) and
[SystemC 2.3.2](http://www.accellera.org/downloads/standards/systemc)
(with [Transaction-Level Modeling Library (TLM 2.0.3)](https://www.doulos.com/knowhow/systemc/tlm2/))

## Requirements

`src/CMakeLists.txt` contains all the requirements to build the library. 

## Build the prototype
Run `make` on the top level Makefile

The default recipe will create a `build` directory and run the project's CMake. This will build all the development
SystemC executables and the SST shared objects.
