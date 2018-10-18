# SystemC-SST Interface

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
