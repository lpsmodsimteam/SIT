/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_galois_lfsr.hpp"
#include <sst/core/sst_config.h>

// Component Constructor
sst_galois_lfsr::sst_galois_lfsr(SST::ComponentId_t id, SST::Params &params)
        : SST::Component(id), m_context(1), m_socket(m_context, ZMQ_REP) {

    // Initialize output
    m_output.init("\033[32mgalois_lfsr-" + getName() + "\033[0m (pid: " + std::to_string(getpid()) + ") -> ", 1, 0,
                  SST::Output::STDOUT);

    m_proc = params.find<std::string>("proc", "");

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_galois_lfsr>(this, &sst_galois_lfsr::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_galois_lfsr::~sst_galois_lfsr() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying master...\n");

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_galois_lfsr::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Master pid: " << getpid() << std::endl;

    if (!fork()) {

        char *args[] = {&m_proc[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                         m_proc.c_str());
        execvp(args[0], args);

    } else {

        //socket.bind ("tcp://*:5555");
        m_socket.bind("ipc:///tmp/zero");

        zmq::message_t request;

        //  Wait for next request from client
        m_socket.recv(&request);
        printf("Received request from client: %s\n", (char *) request.data());

//        //  Send reply back to client
//        zmq::message_t reply((void *) "SHHHH", 5, nullptr);
//        printf("Sending acknowledgement to client...\n");
//        m_socket.send(reply);

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_galois_lfsr::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_galois_lfsr::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    m_data_out["on"] = true;
    m_data_out["reset"] = true;

    // turn module off at 52 ns
    if (current_cycle >= 3) {
        if (current_cycle == 3) {
            std::cout << "RESET OFF" << std::endl;
        }
        m_data_out["reset"] = false;
    }

    // turn module off at 52 ns
    if (current_cycle >= 38) {
        if (current_cycle == 38) {
            std::cout << "GALOIS LFSR MODULE OFF" << std::endl;
        }
        m_data_out["on"] = false;
    }

    char *s = &(m_data_out.dump())[0u];

    std::cout << "MASTER SENDING " << m_data_out.dump() << std::endl;
    zmq::message_t request((void *) "HELLO1", BUFSIZE, nullptr);
    m_socket.send(request);

    zmq::message_t reply;
    m_socket.recv(&reply, 0);
//    m_data_in = json::parse(std::string((char *) reply.data(), reply.size()));
    std::cout << "MASTER RECEIVING " << reply.data() << std::endl;

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}