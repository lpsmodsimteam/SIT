# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A library to provide interoperability between
[Structural Simulation Toolkit (SST) 8.0.0](https://github.com/sstsimulator/sst-core)
(with [SST Elements 8.0.0](https://github.com/sstsimulator/sst-elements)) and
[SystemC 2.3.3](http://www.accellera.org/downloads/standards/systemc)
(with [Transaction-Level Modeling Library (TLM 2.0.3)](https://www.doulos.com/knowhow/systemc/tlm2/))

## Requirements

`src/CMakeLists.txt` contains all the requirements to build the library. 

### SystemC
Sources for stable SystemC releases can be downloaded [here](https://www.accellera.org/downloads/standards/systemc).

Download and untar the source:
```shell
tar -xvf systemc-<ver>.tar.gz
```

Install using CMake
```shell
mkdir build && cd build
cmake -DCMAKE_CXX_STANDARD=11 ..
make && make check
sudo make install
```

### SST
Sources for stable SST releases can be downloaded [here](http://sst-simulator.org/SSTPages/SSTMainDownloads/).

Install the dependencies
```shell
sudo apt install libopenmpi-dev
```

Download and untar the sources:
```shell
tar -xvf sstcore-<ver>.tar.gz && tar -xvf sstelements-<ver>.tar.gz
```

Build and install SST Core
```shell
./configure
make all
sudo make install
```

Build and install SST Elements Library
```shell
./configure
make all
sudo make install
```

Initialize the library
```shell
mkdir ~/.sst
touch ~/.sst/sstsimulator.conf
```

### cppzmq
`libzmq` is required to build cppzmq. Sources for stable libzmq releases can be downloaded [here](https://github.com/zeromq/libzmq), and zppzmq [here](https://github.com/zeromq/cppzmq).

Build and install the library:
```shell
cd libzmq
mkdir build && cd build
cmake ..
sudo make -j4 install

cd cppzmq
mkdir build && cd build
cmake ..
sudo make -j4 install
```

### msgpack
Sources for stable msgpack releases can be downloaded [here](https://github.com/msgpack/msgpack-c).

```shell
cd msgpack-c
cmake -DMSGPACK_CXX11=ON .
sudo make install
```

## Build the prototype
Run `make` on the top level Makefile

The default recipe will create a `build` directory and run the project's CMake. This will build all the development
SystemC executables and the SST shared objects.
