/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "fib_sock.hpp"
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

// Component Constructor
fib_lfsr::fib_lfsr(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id), m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      m_din_link(configureLink(
          "fib_din", new SST::Event::Handler<fib_lfsr>(this, &fib_lfsr::handle_event)
      )),
      m_dout_link(configureLink("fib_dout")) {

    // Initialize output
    m_output.init("\033[32mblackbox-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<fib_lfsr>(this, &fib_lfsr::tick));

    // Configure our reset
    if (!(m_din_link && m_dout_link)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure reset 'reset'\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void fib_lfsr::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process \"%s\" (pid: %d)...\n", m_proc.c_str(), getpid());
        execvp(args[0], args);

    } else {

        m_signal_io.set_addr(m_ipc_port);
        m_signal_io.recv();
        if (child_pid == m_signal_io.get<int>("pid")) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void fib_lfsr::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void fib_lfsr::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";

        m_signal_io.set("reset", std::stoi(_data_in.substr(2, 1)));
        m_signal_io.set("clock", std::stoi(_data_in.substr(3, 2)), SC_UINT_T);

        std::cout << "<----------------------------------------------------" << std::endl;

        m_signal_io.set_state(true);
        if (keep_send) {
            if (!keep_recv) {
                m_signal_io.set_state(false);
            }
            m_signal_io.send();
        }
        if (keep_recv) {
            m_signal_io.recv();
        }

        std::string _data_out = std::to_string(m_signal_io.get<int>("data_out"));
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

        std::cout << "---------------------------------------------------->" << std::endl;

    }

    delete ev;

}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool fib_lfsr::tick(SST::Cycle_t) {

    return false;

}
