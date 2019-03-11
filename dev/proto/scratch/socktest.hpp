#ifndef SOCKTEST_HPP
#define SOCKTEST_HPP

#include "../sigutils1.hpp"

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>


class socktest : public SST::Component {

public:

    socktest(SST::ComponentId_t, SST::Params &);

    ~socktest() override;

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        socktest, // class
        "socktest", // element library
        "socktest", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Simple 4-bit Galois Linear Feedback Shift Register",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

private:

    SST::Link *port;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;

   SignalReceiver m_sh_in;
//    SignalTransmitter m_sh_out;

    size_t bufferSize;
    char buffer[1024];
    int fd, sock, valread;
    struct sockaddr_un addr;
    ssize_t len;
    socklen_t addrlen = sizeof(addr);


};

#endif
