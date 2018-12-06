/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "sst_counter.h"

// Component Constructor
sst_counter::sst_counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // up_counter = params.find<uint8_t>("upCounter", 0);

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_counter>(this, &sst_counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

sst_counter::~sst_counter() {}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_counter::setup() {
    m_up_counter = 0;
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t currentCycle) {

    m_output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
                     static_cast<uint8_t>(m_up_counter));

    m_up_counter++;
    std::cout << currentCycle << '\n';
    return false;
}
