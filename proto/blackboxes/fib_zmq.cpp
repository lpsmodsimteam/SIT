/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "fib_zmq.hpp"
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

// Component Constructor
fib_lfsr::fib_lfsr(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id), m_context(1), m_socket(m_context, ZMQ_REP),
      m_sh_in(m_socket), m_sh_out(m_socket),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      clock(configureLink(
          "fib_clock", new SST::Event::Handler<fib_lfsr>(this, &fib_lfsr::handle_clock)
      )),
      reset(configureLink(
          "fib_reset", new SST::Event::Handler<fib_lfsr>(this, &fib_lfsr::handle_reset)
      )),
      data_out(configureLink("fib_data_out")),
      sim_time(39) {

    // Initialize output
    m_output.init("\033[32mblackbox-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<fib_lfsr>(this, &fib_lfsr::tick));

    // Configure our reset
    if (!reset) {
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

    std::cout << "Master pid: " << getpid() << std::endl;

    if (!fork()) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                         m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_socket.bind(m_ipc_port.c_str());

        m_sh_in.recv();
        std::cout << "[pid]=" << m_sh_in.get<int>("pid") << std::endl;

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void fib_lfsr::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying fib_lfsr...\n");
    m_socket.close();

}

// Receive events that contain the CarType, add the cars to the queue
void fib_lfsr::handle_reset(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_sh_out.set("reset", std::stoi(se->getString()));
    }
    delete ev;
}

// Receive events that contain the CarType, add the cars to the queue
void fib_lfsr::handle_clock(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_sh_out.set("clock", std::stoi(se->getString()), SC_UINT_T);
    }
    delete ev;
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool fib_lfsr::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    bool keep_send = current_cycle < sim_time;
    bool keep_recv = current_cycle < sim_time - 1;

    m_sh_out.set_state(true);

    if (keep_send) {

        if (current_cycle == sim_time - 1) {
            m_output.verbose(CALL_INFO, 1, 0, "FIB LFSR MODULE OFF\n");
            m_sh_out.set_state(false);
        }
        m_sh_out.send();

    }

    if (keep_recv) {
        m_sh_in.recv();
    }

    data_out->send(new SST::Interfaces::StringEvent(
        "\033[34m" + getName() + "\033[0m -> " + std::to_string(m_sh_in.get<int>("data_out"))));

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}
