#ifndef SST_GALOIS_LFSR_HPP
#define SST_GALOIS_LFSR_HPP

#include "../sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>


class galois_lfsr : public SST::Component {

public:

    galois_lfsr(SST::ComponentId_t, SST::Params &);

    ~galois_lfsr() override;

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handleEvent(SST::Event *);
    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        galois_lfsr, // class
        "proto", // element library
        "galois_lfsr", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Simple 4-bit Galois Linear Feedback Shift Register",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "link_galois", "Port on which cars are sent", { "sst.Interfaces.StringEvent" }}
    )

private:

    SST::Link *port;
    int sim_time;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;

    //  Prepare our context and socket
    zmq::context_t m_context;
    zmq::socket_t m_socket;

    SignalReceiver m_sh_in;
    SignalTransmitter m_sh_out;

};

#endif
