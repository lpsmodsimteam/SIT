#ifndef _counter_H
#define _counter_H

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

// hardcoded path
#include "../../kernels/systemc/systemc-2.3.2/examples/sysc/counter/json.hpp"
using json = nlohmann::json;


#define BUFSIZE 256

class sst_counter : public SST::Component {

public:

    sst_counter(SST::ComponentId_t, SST::Params &);

    ~sst_counter();

    void setup();

    void finish();

    bool tick(SST::Cycle_t);

    // int sc_main ();
    // Register the component
    SST_ELI_REGISTER_COMPONENT(
            sst_counter, // class
            "sst_counterSST", // element library
            "sst_counter", // component
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Simple 8-bit Up-Counter Model with one clock",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

private:

    // local variables
    SST::Output m_output;

    json m_data_in;
    json m_data_out;

    struct sockaddr_in serv_addr, cli_addr;
    int portno = 8080;
    int m_sockfd, m_newsockfd;
    char m_buffer[BUFSIZE];

};

#endif
