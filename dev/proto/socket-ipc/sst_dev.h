#ifndef _counter_H
#define _counter_H

#include "sstsysc.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <vector>


class sst_dev : public SST::Component {

public:

    sst_dev(SST::ComponentId_t, SST::Params &);

    ~sst_dev();

    int init_socks();

    int sync_procs();

    void setup();

    void finish();

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
            sst_dev, // class
    "sst_devSST", // element library
    "sst_dev", // component
    SST_ELI_ELEMENT_VERSION(1, 0, 0),
    "Simple 4-bit Up-Counter Model with one clock",
    COMPONENT_CATEGORY_UNCATEGORIZED
    )

private:

    // parameters
    uint16_t m_port;
    std::string m_sysc_counter;
    std::string m_sysc_inverter;

    // socket variables
    int m_num_procs;
    std::string m_proc_names[4] = {"counter1", "inverter"};
    json m_procs;
    struct sockaddr_in m_addr;
    fd_set m_read_fds;  // set of socket descriptors

    socklen_t m_clilen;
    int m_master_sock, m_new_sock;
    char m_buffer[BUFSIZE];

    SST::Output m_output;
    json m_data_in;
    json m_data_in1;
    json m_data_out;
    json m_data_out1;

};

#endif
