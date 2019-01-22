#ifndef _counter_H
#define _counter_H

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

// hardcoded path
#include "json.hpp"

using json = nlohmann::json;


#define BUFSIZE 256

template<typename T>
std::string to_string(const T &);

void send_signal(const json &, int);

//json recv_signal(const char, int);

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
    std::string m_sysc_counter;

    // local variables
    SST::Output m_output;
    json m_data_in;
    json m_data_out;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t m_clilen;
    int m_sockfd, m_newsockfd;
    char m_buffer[BUFSIZE];

};

#endif
