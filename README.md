# SST Interoperable
[![Build Status](https://travis-ci.org/sabbirahm3d/ssti.svg?branch=master)](https://travis-ci.org/sabbirahm3d/ssti)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/ssti?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/ssti&amp;utm_campaign=Badge_Grade)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator/sst-core) and external hardware description languages (HDL).

For interoperability with SystemC, see [the SystemC README](/docs/systemc.md)

## Table of Contents

- [Usage](#usage)
  - [Black Box Code Generation](#black-box-code-generation)
  - [Tests](#tests)
- [Installation](#installation)
  - [CMake](#cmake)
  - [SST Core](#sst-core)
- [Requirements](#requirements)
  - [Languages and Compilers](#languages-and-compilers)
  - [Libraries](#libraries)
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
│   ├── demo_comp.cpp     # SystemC black box driver file
│   └── demo_driver.cpp   # SST black box component
├── demo.cpp              # SST parent component
├── demo_sysc.hpp         # SystemC module
├── build/                # CMake executables
├── CMakeLists.txt        # local CMakeLists for demo
├── tests/                # SST Python scripts
│   └── run.py            # sample SST Python scripts
├── Makefile              # local Makefile
├── .                     # other files
│   .
│   .
```

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
