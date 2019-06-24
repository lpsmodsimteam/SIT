/**
Simple Model with one clock that functions as a wrapper for a PyRTL simulation

Uses named pipes to send and receive data between SST and PyRTL
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "trafficLight.h"

using SST::Interfaces::StringEvent;

trafficLight::trafficLight( SST::ComponentId_t id, SST::Params& params ) :
SST::Component(id) {

	output.init("trafficLight-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);
	
	// Collect all the parameters from the project driver
	clock = params.find<std::string>("clock", "1Hz");
	greenTime  = params.find<int>("greenTime", 30);
	yellowTime = params.find<int>("yellowTime", 3);
	redTime = params.find<int>("redTime", 33);
	startGreen = params.find<int>("startGreen", 0);
	inputPipe = params.find<std::string>("inputPipe", "/tmp/output");
	outputPipe = params.find<std::string>("outputPipe", "/tmp/input");
	
	// Check parameters
	if( (greenTime <= 0) || (yellowTime <= 0) || (redTime <= 0)) {
		output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
	}
	
	// Print parameters
	output.verbose(CALL_INFO, 1, 0, "greenTime=%d, yellowTime=%d, redTime=%d, startGreen=%d\n",
	                                 greenTime, yellowTime, redTime, startGreen);

	// Just register a plain clock for this simple example
	registerClock(clock, new SST::Clock::Handler<trafficLight>(this, &trafficLight::clockTick));
	
	// Configure our port
	port = configureLink("port", new SST::Event::Handler<trafficLight>(this, &trafficLight::handleEvent));
	if ( !port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
	}
}

trafficLight::~trafficLight() {

}

void trafficLight::setup() {
	// Connect to the named pipes when they are available
	while(access(inputPipe.c_str(), R_OK) != 0){}
	inFifo = open(inputPipe.c_str(), O_RDONLY);
	while(access(outputPipe.c_str(), W_OK) != 0){}
	outFifo = open(outputPipe.c_str(), O_WRONLY);
	
	// Load the initial times into the PyRTL stopLight
	sprintf(s, "1%x%02x%02x%02x", startGreen, greenTime, yellowTime, redTime);
	write(outFifo, s, 8);
	char r[2] = "\0";
	read(inFifo, r, 1);
	strncpy(s, "00000000", 8);
}

void trafficLight::finish() {
	// Sent the quit signal to the PyRTL stopLight
	write(outFifo, "q", 1);
	close(inFifo);
	close(outFifo);
}

// Send a command to the PyRTL stopLight every clock
bool trafficLight::clockTick( SST::Cycle_t currentCycle ) {
	write(outFifo, s, 8);
	// Clear the command so we don't send the same command over and over
	// need to receive a command from the port
	strncpy(s, "00000000", 8);
	
	// Read from the PyRTL stopLight to see what state the light is in
	char r[2] = "\0";
	read(inFifo, r, 1);
	std::string c;
	switch(r[0]){
		case '0':
			c = "green";
			break;
		case '1':
			c = "yellow";
			break;
		case '2':
			c = "red";
			break;
	}
	port->send(new StringEvent(c));
	return false;
}

// Send commands that are received to the PyRTL stopLight
void trafficLight::handleEvent(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		strncpy(s, se->getString().c_str(), 8);
	}
	delete ev;
}
