/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "galois_sock.hpp"
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

// Component Constructor
GaloisLFSRSock::GaloisLFSRSock(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id), m_ss(socket(AF_UNIX, SOCK_STREAM, 0)),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      clock(configureLink(
          "galois_clock", new SST::Event::Handler<GaloisLFSRSock>(this, &GaloisLFSRSock::handle_clock)
      )),
      reset(configureLink(
          "galois_reset", new SST::Event::Handler<GaloisLFSRSock>(this, &GaloisLFSRSock::handle_reset)
      )),
      data_out(configureLink("galois_data_out")),
      sim_time(39) {

    // Initialize output
    m_output.init("\033[32mblackbox-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<GaloisLFSRSock>(this, &GaloisLFSRSock::tick));

    // Configure our reset
    if (!reset) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure reset 'reset'\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

GaloisLFSRSock::~GaloisLFSRSock() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying GaloisLFSRSock...\n");

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void GaloisLFSRSock::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Master pid: " << getpid() << std::endl;

    if (!fork()) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                         m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_ss.set_addr(m_ipc_port);
        m_ss.recv();
        std::cout << "[pid]=" << m_ss.get<int>("pid") << std::endl;

        m_output.verbose(CALL_INFO, 1, 0, "Launched black box and connected to socket\n");

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void GaloisLFSRSock::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying GaloisLFSRSock...\n");

}

// Receive events that contain the CarType, add the cars to the queue
void GaloisLFSRSock::handle_reset(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_ss.set("reset", std::stoi(se->getString()));
    }
    delete ev;
}

// Receive events that contain the CarType, add the cars to the queue
void GaloisLFSRSock::handle_clock(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_ss.set("clock", std::stoi(se->getString()), SC_UINT_T);
    }
    delete ev;
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool GaloisLFSRSock::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    bool keep_send = current_cycle < sim_time;
    bool keep_recv = current_cycle < sim_time - 1;

    m_ss.set_state(true);

    if (keep_send) {

        if (current_cycle == sim_time - 1) {
            m_output.verbose(CALL_INFO, 1, 0, "GALOIS LFSR MODULE OFF\n");
            m_ss.set_state(false);
        }
        m_ss.send();

    }

    if (keep_recv) {
        m_ss.recv();
    }

    data_out->send(new SST::Interfaces::StringEvent(
        "\033[34m" + getName() + "\033[0m -> " + std::to_string(m_ss.get<int>("data_out"))));

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}
