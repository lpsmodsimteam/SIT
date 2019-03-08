#ifndef SST_fib_LFSR_HPP
#define SST_fib_LFSR_HPP

#include "../sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>


class fib_lfsr : public SST::Component {

public:

    fib_lfsr(SST::ComponentId_t, SST::Params &);

    ~fib_lfsr() override;

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_reset(SST::Event *);

    void handle_clock(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        fib_lfsr, // class
        "proto", // element library
        "fib_lfsr", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Simple 4-bit fib Linear Feedback Shift Register",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "fib_clock", "Fibonacci LFSR clock", { "sst.Interfaces.StringEvent" }},
        { "fib_reset", "Fibonacci LFSR reset", { "sst.Interfaces.StringEvent" }},
        { "fib_data_out", "Fibonacci LFSR data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    SST::Link *clock, *reset, *data_out;
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
