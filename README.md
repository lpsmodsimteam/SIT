# SystemC-SST Interoperability
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/systemc-sst?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/systemc-sst&amp;utm_campaign=Badge_Grade)

## Kernels

  - [SystemC 2.3.2](http://www.accellera.org/downloads/standards/systemc), with [Transaction-Level Modeling Library (TLM 2.0.3)](https://www.doulos.com/knowhow/systemc/tlm2/)
  - [Structural Simulation Toolkit (SST) 8.0.0](https://github.com/sstsimulator/sst-core), with [SST Elements 8.0.0](https://github.com/sstsimulator/sst-elements)

## Environment

  - C++: `std=c++1y` 

  - Compiler: `g++ 7.3.0` or `clang 6.0.0-1ubuntu2` 

  - CMake 3.10.2

  - SystemC

    - `SYSTEMC_FLAGS = -I. -I${SYSTEMC_HOME}/include -L. -L${SYSTEMC_HOME}/lib-linux64 -Wl,-rpath=${SYSTEMC_HOME}/lib-linux64 -lsystemc -lm`

  - SST

    - `ELEMENT_CXXFLAGS = -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I${SST_ELEMENTS_HOME}/include`

    - `ELEMENT_LDFLAGS = -shared -fno-common -Wl,-undefined -Wl,dynamic_lookup`
