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
      m_din_link(configureLink(
          "galois_din", new SST::Event::Handler<GaloisLFSRSock>(this, &GaloisLFSRSock::handle_event)
      )),
      m_dout_link(configureLink("galois_dout")) {

    // Initialize output
    m_output.init("\033[32mblackbox-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<GaloisLFSRSock>(this, &GaloisLFSRSock::tick));

    // Configure our reset
    if (!(m_din_link && m_dout_link)) {
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

void GaloisLFSRSock::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";
        int reset_val = std::stoi(_data_in.substr(2, 1));
        int clock_val = std::stoi(_data_in.substr(3, 2));

        m_ss.set("reset", reset_val);
        m_ss.set("clock", clock_val, SC_UINT_T);

        std::cout << "<----------------------------------------------------" << std::endl;

        m_ss.set_state(true);

        if (keep_send) {

            if (!keep_recv) {
                m_ss.set_state(false);
            }
            m_ss.send();

        }

        if (keep_recv) {
            m_ss.recv();
        }

        std::string _data_out = "\033[34m" + getName() + "\033[0m -> "
                                + std::to_string(m_ss.get<int>("data_out"));
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

        std::cout << "---------------------------------------------------->" << std::endl;

    }

    delete ev;

}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool GaloisLFSRSock::tick(SST::Cycle_t) {

    return false;

}
