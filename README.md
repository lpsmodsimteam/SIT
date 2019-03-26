# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A library to provide interoperability between
[Structural Simulation Toolkit (SST) 8](https://github.com/sstsimulator/sst-core)
(with [SST Elements 8](https://github.com/sstsimulator/sst-elements)) and
[SystemC 2.3](http://www.accellera.org/downloads/standards/systemc)
(with [Transaction-Level Modeling Library (TLM 2)](https://www.doulos.com/knowhow/systemc/tlm2/))

## Requirements

The following table summarizes the minimum versions of the languages and libraries used in the project.

|Requirement|Version|
|-----------|-------|
|C++        |11     |
|CMake      |3.14   |
|msgpack    |3.1.1  |
|Python     |3.6    |
|SST        |8.0.0  |
|SystemC    |2.3.3  |

### System

Due to limitations with SST, this library currently only works on Linux systems. A list of supported distributions can be found in `.travis.yml`.
