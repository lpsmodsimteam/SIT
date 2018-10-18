/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
#include "systemc.h"
#include "counter.h"

// Component Constructor
counter::counter( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id), upCounter(0) {

	// Initialize output
	output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	upCounter = params.find<uint8_t>("upCounter", 0);

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
	output.verbose(CALL_INFO, 1, 0, "Component is being setup.\n");
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void counter::finish() {
	output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool counter::tick( SST::Cycle_t currentCycle ) {

	output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
		static_cast<uint8_t>(upCounter));

	upCounter++;
	return false;
}
