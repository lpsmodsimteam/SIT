# SST-SystemC Interoperability Toolkit
[![Build Status](https://travis-ci.org/sabbirahm3d/sstscit.svg?branch=master)](https://travis-ci.org/sabbirahm3d/sstscit)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/88c38abb1f2a4a369b4a6f9c49e8d237)](https://www.codacy.com/app/sabbirahm3d/sstscit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=sabbirahm3d/sstscit&amp;utm_campaign=Badge_Grade)

A toolkit to provide interoperability between [Structural Simulation Toolkit (SST 8)](https://github.com/sstsimulator/sst-core) (with [SST Elements 8](https://github.com/sstsimulator/sst-elements)) and [SystemC 2.3](http://www.accellera.org/downloads/standards/systemc) (with [Transaction-Level Modeling Library (TLM 2)](https://www.doulos.com/knowhow/systemc/tlm2/))

For further details on the implementation, see the [system design document](/docs/sstscit.pdf).

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

Due to limitations with SST, this library currently only works on Linux systems. A list of supported distributions can be found in `.travis.yml`.

## Usage

To establish interoperability between an SST model and a SystemC module, a black box SST model and SystemC driver must be generated. The black boxes establish interprocess communication (IPC) between the SST and SystemC processes. The Python class `generate.BoilerPlate` can be used to generate the black boxes.

<!-- 
    The following snippets demonstrate an SST link transmitting a unidirectional signal from the SST
    environment to the black box interface in an example component, \lstinline{demo}.

\begin{lstlisting}[language=C++, caption={Snippet of \lstinline{demo.cpp}}, captionpos=b]
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
\end{lstlisting}

\begin{lstlisting}[language=C++, caption={Snippet of \lstinline{blackboxes/demo_comp.cpp}}, captionpos=b]
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
\end{lstlisting}

 -->