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

    std::cout << "after\n";
    m_up_counter = 0;
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    close(m_newsockfd);
    close(m_sockfd);
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t currentCycle) {

    m_output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
                     static_cast<uint8_t>(m_up_counter));
    if (m_newsockfd < 0) {
        perror("ERROR on accept");
    }

    char buffer[256];
    bzero(buffer, 256);
    int n = read(m_newsockfd, buffer, 255);

    if (n < 0) {
        perror("ERROR reading from socket");
    }
    printf("Here is the message: %s\n", buffer);

    n = write(m_newsockfd, "I got your message", 18);

    if (n < 0) {
        perror("ERROR writing to socket");
    }

    m_up_counter++;
//    std::cout << currentCycle << '\n';
    return false;
}
