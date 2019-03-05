/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_fib_lfsr.hpp"
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

// Component Constructor
sst_fib_lfsr::sst_fib_lfsr(SST::ComponentId_t id, SST::Params &params)
        : SST::Component(id), m_context(1), m_socket(m_context, ZMQ_REP),
          m_sh_in(m_socket), m_sh_out(m_socket) {

    // Initialize output
    m_output.init("\033[32mgalois_lfsr-" + getName() + "\033[0m (pid: " + std::to_string(getpid()) + ") -> ", 1, 0,
                  SST::Output::STDOUT);

    m_proc = params.find<std::string>("proc", "");
    m_port = params.find<std::string>("_port", "");

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_fib_lfsr>(this, &sst_fib_lfsr::tick));

    // Configure our port
    port = configureLink("port1", new SST::Event::Handler<sst_fib_lfsr>(this, &sst_fib_lfsr::handleEvent));
    if (!port) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_fib_lfsr::~sst_fib_lfsr() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying master...\n");
    m_socket.close();

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_fib_lfsr::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Master pid: " << getpid() << std::endl;

    if (!fork()) {

        char *args[] = {&m_proc[0u], &m_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                         m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_socket.bind(&m_port[0u]);

        m_sh_in.recv();
        std::cout << "[pid]=" << m_sh_in.get<int>("pid") << std::endl;

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_fib_lfsr::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// Receive events that contain the CarType, add the cars to the queue
void sst_fib_lfsr::handleEvent(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << se->getString() << std::endl;
    }
    delete ev;
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_fib_lfsr::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    bool keep_send, keep_recv;
    keep_send = current_cycle < 39;
    keep_recv = current_cycle < 38;

    m_sh_out.set("clock", current_cycle, SC_UINT_T);
    m_sh_out.set_state(true);
    m_sh_out.set("reset", 1);

    // turn module off at 52 ns
    if (current_cycle >= 3) {
        if (current_cycle == 3) {
            std::cout << "RESET OFF" << std::endl;
        }
        m_sh_out.set("reset", 0);
    }

    // turn module off at 52 ns
    if (current_cycle >= 38) {
        if (current_cycle == 38) {
            std::cout << "GALOIS LFSR MODULE OFF" << std::endl;
        }
        m_sh_out.set_state(false);
    }

    if (keep_send) {

        m_sh_out.send();
    }

    if (keep_recv) {

        m_sh_in.recv();
        m_output.verbose(CALL_INFO, 1, 0, "%d\n", m_sh_in.get<int>("fib_lfsr"));
    }

    port->send(new SST::Interfaces::StringEvent("fib " + std::to_string(m_sh_in.get<int>("fib_lfsr"))));

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}
