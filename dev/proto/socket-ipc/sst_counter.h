#ifndef _counter_H
#define _counter_H

#include "sstsysc.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <vector>


class sst_counter : public SST::Component {

public:

    sst_counter(SST::ComponentId_t, SST::Params &);

    ~sst_counter();

    void setup();

    void finish();

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
            sst_counter, // class
            "sst_counterSST", // element library
            "sst_counter", // component
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Simple 4-bit Up-Counter Model with one clock",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

private:

    // parameters
    uint16_t m_port;
    std::string m_sysc_counter1, m_sysc_counter2;

    // local variables
    std::vector<pid_t> pids;
    SST::Output m_output;
    json m_data_in;
    json m_data_out;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t m_clilen;
    int m_sockfd, m_newsockfd;
    char m_buffer[BUFSIZE];

};

#endif
