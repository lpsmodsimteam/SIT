# SystemC-SST Interoperability Library
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/systemc-sst?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/systemc-sst&amp;utm_campaign=Badge_Grade)

A library to provide interoperability between
[Structural Simulation Toolkit (SST) 8.0.0](https://github.com/sstsimulator/sst-core)
(with [SST Elements 8.0.0](https://github.com/sstsimulator/sst-elements)) and
[SystemC 2.3.3](http://www.accellera.org/downloads/standards/systemc)
(with [Transaction-Level Modeling Library (TLM 2.0.3)](https://www.doulos.com/knowhow/systemc/tlm2/))

## Requirements

`src/CMakeLists.txt` contains all the requirements to build the library. 
- C++14
- OpenMPI / MPICH
- [SystemC 2.3.3](http://www.accellera.org/downloads/standards/systemc)
- [Structural Simulation Toolkit (SST) 8.0.0](https://github.com/sstsimulator/sst-core)

### CMake
Sources for stable CMake releases can be downloaded [here](https://cmake.org/download/).

Download and untar the source:
```tar -xvf cmake-<ver>.tar.gz```

Run the bootstrap script, then install
```shell
./bootstrap
make
sudo make install
```

### SystemC
Sources for stable SystemC releases can be downloaded [here](https://www.accellera.org/downloads/standards/systemc).

Download and untar the source:
```tar -xvf systemc-<ver>.tar.gz```

Install using CMake and CCMake
```shell
# to install ccmake
sudo apt install cmake-curses-gui
mkdir build && cd build
# run CCMake to configure the file generation to change the C++ standard from 98 to 14
ccmake ..
make && make check
sudo make install
```

### SST

Install the dependencies
```sudo apt install libopenmpi-dev autoconf libtool-bin```

Download and untar the sources:
```shell
tar -xvf sstcore-<ver>.tar.gz && tar -xvf sstelements-<ver>.tar.gz
```

Set up the environment variables
```shell
# sst simulator start #
export SST_CORE_HOME=$HOME/local/sstcore-<ver>
export SST_ELEMENTS_HOME=$HOME/local/sstelements-<ver>
export PATH=$SST_CORE_HOME/bin:$SST_ELEMENTS_HOME/bin:$PATH
# sst simulator stop #
```

Build and install SST Core
```shell
./configure --prefix=$SST_CORE_HOME
make all
sudo make install
```

Build and install SST Elements Library
```shell
./configure --prefix=$SST_ELEMENTS_HOME --with-sst-core=$SST_CORE_HOME
make all
sudo make install
```

## Build the prototype
Run `make` on the top level Makefile

The default recipe will create a `build` directory and run the project's CMake. This will build all the development
SystemC executables and the SST shared objects.
