#ifndef _SST_DEV_H
#define _SST_DEV_H

#include "sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>


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

    // SST parameters
    SST::Output m_output;
    std::string m_sysc_counter;
    std::string m_sysc_inverter;
    std::string m_sc_lrs;
    std::string m_sc_galois_lfsr;
    int m_num_procs;

    // process variables
    json m_procs;  // all details about child processes
    MPI_Comm m_inter_com;
    char (*send_buf)[BUFSIZE];
    char (*recv_buf)[BUFSIZE];
    json m_data_in;
    json m_data_out;

};

#endif