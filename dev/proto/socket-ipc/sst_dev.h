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
    std::string m_sysc_counter;
    std::string m_sysc_inverter;
    int m_num_procs;
    std::string *m_proc_names;
    json m_procs;

    int *np;
    int *errcodes;
    MPI_Comm m_inter_com;
    char **cmds;
    MPI_Info *infos;

    char (*send_buf)[BUFSIZE];
    char (*recv_buf)[BUFSIZE];

    SST::Output m_output;
    json m_data_out;

};

#endif
