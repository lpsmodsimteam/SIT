/*
 * Parent SST model
 *
 * */

#include "prototype.hpp"
#include <sst/core/sst_config.h>

#define SIMTIME "39"

prototype::prototype(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_clock(params.find<std::string>("clock", "")),
      galois_reset(configureLink("galois_reset")),
      galois_clock(configureLink("galois_clock")),
      galois_data_out(configureLink(
          "galois_data_out",
          new SST::Event::Handler<prototype>(this, &prototype::handle_galois_data_out)
      )),
      fib_reset(configureLink("fib_reset")),
      fib_clock(configureLink("fib_clock")),
      fib_data_out(configureLink(
          "fib_data_out", new SST::Event::Handler<prototype>(this, &prototype::handle_fib_data_out)
      )) {

    // Initialize output
    m_output.init("\033[34mparent_model-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Configure our port
    if (!(galois_reset && fib_reset)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<prototype>(this, &prototype::tick));
    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

prototype::~prototype() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying master...\n");

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void prototype::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Parent SST pid: " << getpid() << std::endl;

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void prototype::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

void prototype::handle_galois_data_out(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << se->getString() << std::endl;
    }
    delete ev;
}

void prototype::handle_fib_data_out(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << se->getString() << std::endl;
    }
    delete ev;
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool prototype::tick(SST::Cycle_t current_cycle) {

    std::string galois_reset_sig, fib_reset_sig;
    // turn reset off at 3 ns
    if (current_cycle >= 3) {

        if (current_cycle == 3) {
            m_output.verbose(CALL_INFO, 1, 0, "RESET OFF\n");
        }

        galois_reset_sig = "0";
        fib_reset_sig = "0";

    } else {

        galois_reset_sig = "1";
        fib_reset_sig = "1";

    }

    galois_clock->send(new SST::Interfaces::StringEvent(std::to_string(current_cycle)));
    fib_clock->send(new SST::Interfaces::StringEvent(std::to_string(current_cycle)));

    galois_reset->send(new SST::Interfaces::StringEvent(galois_reset_sig));
    fib_reset->send(new SST::Interfaces::StringEvent(fib_reset_sig));

    return false;

}
