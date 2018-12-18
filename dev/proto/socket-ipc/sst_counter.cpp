/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "sst_counter.h"


// Component Constructor
sst_counter::sst_counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // up_counter = params.find<uint8_t>("upCounter", 0);

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_counter>(this, &sst_counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_counter::~sst_counter() {}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_counter::setup() {

    char *args[] = {"./../../kernels/systemc/systemc-2.3.2/examples/sysc/counter/counter", nullptr};
    m_data_out["on"] = true;
    m_data_out["clock"] = 0;
    m_data_out["reset"] = 0;
    m_data_out["enable"] = 0;

    if (!fork()) {

        execvp(args[0], args);

    } else {

        // -------------- SERVER-SIDE -----------------
        m_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (m_sockfd < 0) {
            perror("ERROR opening socket");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        if (bind(m_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
            exit(-1);
        }

        listen(m_sockfd, 5);

        socklen_t clilen = sizeof(cli_addr);

        m_newsockfd = accept(m_sockfd, (struct sockaddr *) &cli_addr, &clilen);

        // -------------- SERVER-SIDE -----------------

    }

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    close(m_newsockfd);
    close(m_sockfd);
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t currentCycle) {

    m_data_out["clock"] = currentCycle;
    if (currentCycle == 10) {
        std::cout << "RESET ON" << std::endl;
        m_data_out["reset"] = 1;
    }

    if (currentCycle == 15) {
        std::cout << "RESET OFF" << std::endl;
        m_data_out["reset"] = 0;
    }

    if (currentCycle == 20) {
        std::cout << "ENABLE ON" << std::endl;
        m_data_out["enable"] = 1;
    }

    if (currentCycle == 570) {
        std::cout << "ENABLE OFF" << std::endl;
        m_data_out["enable"] = 0;
    }

    if (currentCycle == 580) {
        std::cout << "MODULE OFF" << std::endl;
        m_data_out["on"] = false;
    }

    std::string m_data_out_str = to_string(m_data_out);

    if (m_newsockfd < 0) {
        perror("ERROR on accept");
    }

    bzero(m_buffer, BUFSIZE);

    if (read(m_newsockfd, m_buffer, BUFSIZE - 1) < 0) {
        perror("ERROR reading from socket");
    }

    std::cout << "Counter: " << json::parse(m_buffer)["counter_out"] << std::endl;
//    m_output.verbose(CALL_INFO, 1, 0, "Counter: %s \n", json::parse(m_buffer)["counter_out"]);

    if (write(m_newsockfd, m_data_out_str.c_str(), m_data_out_str.size()) < 0) {
        perror("ERROR writing to socket");
    }

    return false;

}
