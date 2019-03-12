#ifndef FIB_SOCK_HPP
#define FIB_SOCK_HPP

#include "../../../src/sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>


class FibLFSRSock : public SST::Component {

public:

    FibLFSRSock(SST::ComponentId_t, SST::Params &);

    ~FibLFSRSock() override;

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_reset(SST::Event *);

    void handle_clock(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        FibLFSRSock, // class
        "proto", // element library
        "fib_lfsr", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Simple 4-bit Fibonacci Linear Feedback Shift Register",
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

    SignalSocket m_ss;

};

#endif
