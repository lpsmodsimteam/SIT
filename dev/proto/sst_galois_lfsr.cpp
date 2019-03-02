/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_galois_lfsr.hpp"
#include <sst/core/sst_config.h>

// Component Constructor
sst_galois_lfsr::sst_galois_lfsr(SST::ComponentId_t id, SST::Params &params)
        : SST::Component(id), m_context(1), m_socket(m_context, ZMQ_REP),
          m_data_in(m_socket), m_data_out(m_socket) {

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
    m_socket.close();

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

        m_socket.bind("ipc:///tmp/zero");

        m_data_in.recv();
        std::cout << "[pid]=" << m_data_in["pid"] << std::endl;

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

    uint8_t keep_send, keep_recv;
    if (current_cycle == 1) {
        keep_send = 1;
        keep_recv = 1;
    }

    m_data_out["clock"] = std::to_string(current_cycle);
    m_data_out["on"] = "1";
    m_data_out["reset"] = "1";

    // turn module off at 52 ns
    if (current_cycle >= 3) {
        if (current_cycle == 3) {
            std::cout << "RESET OFF" << std::endl;
        }
        m_data_out["reset"] = "0";
    }

    // turn module off at 52 ns
    if (current_cycle >= 38) {
        if (current_cycle == 38) {
            std::cout << "GALOIS LFSR MODULE OFF" << std::endl;
        }
        m_data_out["on"] = "0";
        keep_send = 0;
    }


    if (keep_send | keep_recv) {

        m_data_out.send();

        if (!keep_send) {

            keep_recv = 0;

        } else {

            m_data_in.recv();
            m_output.verbose(CALL_INFO, 1, 0, "%s\n", m_data_in["galois_lfsr"].c_str());
        }

    }

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}
