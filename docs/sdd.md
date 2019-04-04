# SST-SystemC Interoperability Toolkit
## System Design Document

This collection of header files provides methods to transmit and receive signals between SST components and SystemC modules. The toolkit provides a black box interface that can be interfaced with both SST and SystemC via their internal communication protocols. 

![comm](https://raw.githubusercontent.com/sabbirahm3d/sstscit/master/docs/comm.png)

### Black Box

The black box interface consists of:
1. a SystemC driver
2. an SST component

Each SystemC modules must have their corresponding driver file to interoperate within the black box interface. It is possible to interoperate multiple SystemC modules with a single driver file. However, the additional communication lines must be accounted for in the corresponding black box SST component.

### Communication

#### Inter-Black Box Communication
The two components inside the black box interface are spawned in the same node and therefore communicate via interprocess communication (IPC) protocols. The following is a list of supported IPC protocols:
1. Unix domain sockets (IPC sockets)
2. ZeroMQ

#### SST-Black Box Communication
An SST model can interface the black box via standard SST links. The following snippets demonstrate an SST link transmitting a unidirectional signal from the SST environment to the black box interface.

```c++
// /parent_sst.cpp

// register a string event in the class declaration
...
SST_ELI_DOCUMENT_PORTS(
    { "demo_din", "Demo model data in", { "sst.Interfaces.StringEvent" }},
    ...
)
...

// initialize the link in the class declaration
...
SST::Link *demo_din;
...

// set up the SST link in the constructor
...
demo_din = configureLink("demo_din");
...

// trigger the event in the clocked function
...
demo_din->send(new SST::Interfaces::StringEvent(...));
...

```

```c++
// blackboxes/demo.cpp

// register the same string event in the class declaration
...
SST_ELI_DOCUMENT_PORTS(
    { "demo_din", "Demo model data in", { "sst.Interfaces.StringEvent" }},
    ...
)
...

// initialize the same link in the class declaration
...
SST::Link *demo_din;
...

// set up the SST link in the constructor with an event handler
...
demo_din = configureLink(
    "demo_din",
    new SST::Event::Handler<demo>(this, &demo::handle_event)
);
...

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

#### SystemC-Black Box Communication
