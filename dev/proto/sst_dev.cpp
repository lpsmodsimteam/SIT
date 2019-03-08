/*
 * Parent SST model
 *
 * */

#include "sst_dev.hpp"
#include <sst/core/sst_config.h>

#define SIMTIME "39"

sst_dev::sst_dev(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_clock(params.find<std::string>("clock", "")),
      galois_reset(configureLink("galois_reset")),
      galois_clock(configureLink("galois_clock")),
      galois_data_out(configureLink(
          "galois_data_out", 
          new SST::Event::Handler<sst_dev>(this, &sst_dev::handle_galois_data_out)
      )),
      link_fib(configureLink(
          "link_fib", new SST::Event::Handler<sst_dev>(this, &sst_dev::handle_fib_lfsr)
      )) {

    // Initialize output
    m_output.init("\033[34mparent_model-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Configure our port
    if (!(galois_reset && link_fib)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<sst_dev>(this, &sst_dev::tick));
    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_dev::~sst_dev() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying master...\n");

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_dev::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Parent SST pid: " << getpid() << std::endl;

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_dev::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

void sst_dev::handle_galois_data_out(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << se->getString() << std::endl;
    }
    delete ev;
}

void sst_dev::handle_fib_lfsr(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << se->getString() << std::endl;
        link_fib->send(new SST::Interfaces::StringEvent(SIMTIME));
    }
    delete ev;
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_dev::tick(SST::Cycle_t current_cycle) {

    std::string galois_reset_sig;
    // turn reset off at 3 ns
    if (current_cycle >= 3) {

        if (current_cycle == 3) {
            m_output.verbose(CALL_INFO, 1, 0, "RESET OFF\n");
        }

        galois_reset_sig = "0";

    } else {

        galois_reset_sig = "1";

    }

    galois_clock->send(new SST::Interfaces::StringEvent(std::to_string(current_cycle)));
    galois_reset->send(new SST::Interfaces::StringEvent(galois_reset_sig));

    return false;

}
