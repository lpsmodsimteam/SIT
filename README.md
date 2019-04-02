# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A toolkit to provide interoperability between
[Structural Simulation Toolkit (SST 8)](https://github.com/sstsimulator/sst-core) (with [SST Elements 8](https://github.com/sstsimulator/sst-elements)) and [SystemC 2.3](http://www.accellera.org/downloads/standards/systemc) (with [Transaction-Level Modeling Library (TLM 2)](https://www.doulos.com/knowhow/systemc/tlm2/))

## Requirements

The following table summarizes the minimum versions of the languages and libraries used in the project.

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

Due to limitations with SST, this library currently only works on Linux systems. A list of supported distributions can be found in `.travis.yml`.

## Usage

To establish interoperability between an SST model and a SystemC module, a black box SST model and SystemC driver must be generated. The black boxes establish interprocess communication (IPC) between the SST and SystemC processes. The Python class `generate.BoilerPlate` can be used to generate the black boxes.

![alt image](https://raw.githubusercontent.com/sabbirahm3d/sstscit/master/docs/model.png)