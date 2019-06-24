/**
Main logic for the intersection

Connects to 2 traffic lights (PyRTL wrapped in SST) and 2 car generators
Keeps track of some statistics for number of cars and largest backups at the lights
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "intersection.h"

using SST::Interfaces::StringEvent;

intersection::intersection( SST::ComponentId_t id, SST::Params& params ) :
SST::Component(id) {

	output.init("intersection-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);
	
	// Collect parameters
	clock = params.find<std::string>("clock", "1Hz");
	simDuration  = params.find<SST::Cycle_t>("simDuration", 200);

	// Error check params
	if( ! (simDuration > 0) ) {
		output.fatal(CALL_INFO, -1, "Error: simDuration must be greater than zero.\n");
	}
	output.verbose(CALL_INFO, 1, 0, "simDuration=%d Hours\n", ((int)simDuration/3600));

	// Just register a plain clock for this simple example
	registerClock(clock, new SST::Clock::Handler<intersection>(this, &intersection::clockTick));
	
	// Configure our ports
	light0 = configureLink("light0", new SST::Event::Handler<intersection>(this, &intersection::handleLight0));
	if ( !light0 ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'light0'\n");
	}
	light1 = configureLink("light1", new SST::Event::Handler<intersection>(this, &intersection::handleLight1));
	if ( !light1 ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'light1'\n");
	}
	cars0 = configureLink("cars0", new SST::Event::Handler<intersection>(this, &intersection::handleCars0));
	if ( !cars0 ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'cars0'\n");
	}
	cars1 = configureLink("cars1", new SST::Event::Handler<intersection>(this, &intersection::handleCars1));
	if ( !cars1 ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'cars1'\n");
	}

	// Tell SST to wait until we authorize it to exit
	registerAsPrimaryComponent();
	primaryComponentDoNotEndSim();
}

intersection::~intersection() {

}

void intersection::setup() {
	road0 = 0;
	road1 = 0;
	totalCars0 = 0;
	totalCars1 = 0;
	backup0 = 0;
	backup1 = 0;
	numtics = 0;
}

void intersection::finish() {
	printf("\nCars that drove through Traffic Light 0: %d\n", totalCars0);
	printf("Cars that drove through Traffic Light 1: %d\n", totalCars1);
	printf("Largest backup at Traffic Light0: %d\n", backup0);
	printf("Largest backup at Traffic Light1: %d\n\n", backup1);
}

// Exit when enough clock ticks have happened
bool intersection::clockTick( SST::Cycle_t currentCycle ) {
	if(currentCycle == 1){
		printf("\n Hour | Total Cars TL0 | Total Cars TL1\n");
		printf("------+----------------+---------------\n");
	}
	numtics++;
	if ((numtics % 3600) == 0){
		printf(" %4d | %14d | %14d\n", numtics/3600, totalCars0, totalCars1);
	}
	if( currentCycle >= simDuration ) {
		primaryComponentOKToEndSim();
		return true;
	} else {
		return false;
	}
}

// If the light is green or yellow, allow cars to go through one at a time
void intersection::handleLight0(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		//output.output("Light0: %s\n", se->getString().c_str());
		if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y'){
			if (road0 > 0){
				road0--;
			}
		}
	}
	delete ev;
}

// If the light is green or yellow, allow cars to go through one at a time
void intersection::handleLight1(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		//output.output("Light1: %s\n", se->getString().c_str());
		if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y'){
			if (road1 > 0){
				road1--;
			}
		}
	}
	delete ev;
}

// Add cars to the road when they arrive and keep track of the largest backup
void intersection::handleCars0(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		if (se->getString().c_str()[0] == '1'){
			road0++;
			totalCars0++;
			if (road0 > backup0){
				backup0 = road0;
			}
		}
	}
	delete ev;
}

// Add cars to the road when they arrive and keep track of the largest backup
void intersection::handleCars1(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		if (se->getString().c_str()[0] == '1'){
			road1++;
			totalCars1++;
			if (road1 > backup1){
				backup1 = road1;
			}
		}
	}
	delete ev;
}
