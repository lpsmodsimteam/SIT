/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "sst_counter.h"


// Component Constructor
sst_counter::sst_counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    m_port = params.find<uint16_t>("port", 8080);
    m_sysc_counter = params.find<std::string>("sysc_counter", "");

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_counter>(this, &sst_counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_counter::~sst_counter() {
    close(m_newsockfd);
    close(m_sockfd);
}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_counter::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    if (!fork()) {

        char *args[] = {&m_sysc_counter[0u], "8080", nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking SystemC process from: %s\n", args[0]);
        execvp(args[0], args);

    } else {

        m_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (m_sockfd < 0) {
            perror("ERROR opening socket");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(m_port);

        if (bind(m_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
            exit(-1);
        }

        listen(m_sockfd, 5);

        m_clilen = sizeof(cli_addr);

        m_newsockfd = accept(m_sockfd, (struct sockaddr *) &cli_addr, &m_clilen);

        m_data_out["on"] = true;
        m_data_out["clock"] = 0;
        m_data_out["reset"] = 0;
        m_data_out["enable"] = 0;

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

template<typename T>
std::string to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

void send_signal(const json &data, int sock_fd) {

    // convert JSON object to bytes to be transmitted via sockets
    std::string data_str = to_string(data);
    if (write(sock_fd, data_str.c_str(), data_str.size()) < 0) {
        perror("ERROR writing to socket");
    }

}

json recv_signal(char buffer[], int sock_fd) {

    if (sock_fd < 0) {
        perror("ERROR on accept");
    }

    // reset buffer
    bzero(buffer, BUFSIZE);

    // read buffer from child process
    if (read(sock_fd, buffer, BUFSIZE - 1) < 0) {
        perror("ERROR reading from socket");
    }

    try {
        return json::parse(buffer);
    } catch (json::parse_error &e) {
        std::cout << "JSON PARSE ERROR" << std::endl;
        return json{};
    }
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t currentCycle) {

    // ---------------- SYSTEMC MODULE TESTBENCH ----------------

    // assign SST clock to SystemC clock
    m_data_out["clock"] = currentCycle;

    // set reset to 1 at 4 ns
    if (currentCycle == 4) {
        std::cout << "RESET ON" << std::endl;
        m_data_out["reset"] = 1;
    }

    // set reset to 0 at 8 ns
    if (currentCycle == 8) {
        std::cout << "RESET OFF" << std::endl;
        m_data_out["reset"] = 0;
    }

    // set enable to 1 at 12 ns
    if (currentCycle == 12) {
        std::cout << "ENABLE ON" << std::endl;
        m_data_out["enable"] = 1;
    }

    // set enable to 0 at 50 ns
    if (currentCycle == 50) {
        std::cout << "ENABLE OFF" << std::endl;
        m_data_out["enable"] = 0;
    }

    // turn module off at 52 ns
    if (currentCycle == 52) {
        std::cout << "MODULE OFF" << std::endl;
        m_data_out["on"] = false;
    }

    // ---------------- SOCKET COMMUNICATION ----------------

    // ---------------- WRITE DATA ----------------
    send_signal(m_data_out, m_newsockfd);

    // if module is on, dump the JSON object buffer
    if (m_data_out["on"]) {

        // ---------------- READ DATA ----------------
        m_data_in = recv_signal(m_buffer, m_newsockfd);

        m_output.verbose(CALL_INFO, 1, 0, "Counter: %s \n",
                         std::string(m_data_in["counter_out"]).c_str());

    }

    return false;

}
