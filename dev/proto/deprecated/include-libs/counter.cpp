/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "counter.h"

// Component Constructor
counter::counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // up_counter = params.find<uint8_t>("upCounter", 0);

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<counter>(this, &counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

counter::~counter() {}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void counter::setup() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
    // sc_main();
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
    sc_main();
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool counter::tick(SST::Cycle_t currentCycle) {

    // output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
    //     static_cast<uint8_t>(upCounter));

    // up_counter++;
    std::cout << currentCycle << '\n';
    return false;
}
