/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_dev.hpp"
#include <sst/core/sst_config.h>

// Component Constructor
sst_dev::sst_dev(SST::ComponentId_t id, SST::Params &params)
        : SST::Component(id) {

    // Initialize output
    m_output.init("\033[31mparent_sst-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_dev>(this, &sst_dev::tick));

    // Configure our port
    port = configureLink("port", new SST::Event::Handler<sst_dev>(this, &sst_dev::handleEvent));
    if (!port) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

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

// Receive events that contain the CarType, add the cars to the queue
void sst_dev::handleEvent(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        std::cout << atoi(&(se->getString().c_str()[0])) << std::endl;
    }
    delete ev;
}


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_dev::tick(SST::Cycle_t current_cycle) {

    return false;

}
