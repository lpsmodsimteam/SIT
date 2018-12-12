/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "sst_counter.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

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

    char *args[]={"./../../kernels/systemc/systemc-2.3.2/examples/sysc/counter/counter", nullptr};
    std::cout << "before\n";
    int child_pid = fork();

    if (!child_pid) {
        execvp(args[0], args);
    } else {

        // -------------- SERVER-SIDE -----------------
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            perror("ERROR opening socket");
        }

        struct sockaddr_in serv_addr, cli_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));

        int portno = 8080;

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
        }

        listen(sockfd, 5);

        socklen_t clilen = sizeof(cli_addr);

        int newsockfd = accept(sockfd,
                               (struct sockaddr *) &cli_addr,
                               &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
        }

        char buffer[256];
        bzero(buffer, 256);
        int n = read(newsockfd, buffer, 255);

        if (n < 0) {
            perror("ERROR reading from socket");
        }
        printf("Here is the message: %s\n", buffer);

        close(newsockfd);
        close(sockfd);
        // -------------- SERVER-SIDE -----------------

    }


    std::cout << "LOL" << child_pid << "\n";
    std::cout << "after\n";
    m_up_counter = 0;
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t currentCycle) {

    m_output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
                     static_cast<uint8_t>(m_up_counter));

    m_up_counter++;
//    std::cout << currentCycle << '\n';
    return false;
}
