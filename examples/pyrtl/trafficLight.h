#ifndef _trafficLight_H
#define _trafficLight_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

class trafficLight : public SST::Component {

public:
	trafficLight( SST::ComponentId_t id, SST::Params& params );
	~trafficLight();

	void setup();
	void finish();

	bool clockTick( SST::Cycle_t currentCycle );

	void handleEvent(SST::Event *ev);

	SST_ELI_REGISTER_COMPONENT(
		trafficLight,
		"intersection",
		"trafficLight",
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"Demonstration of a PyRTL hardware simulation in SST",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	SST_ELI_DOCUMENT_PARAMS(
		{ "clock", "Clock frequency or period", "1Hz" },
		{ "greenTime", "How long the green light stays on", "30" },
		{ "yellowTime", "How long the yellow light stays on", "3" },
		{ "redTime", "How long the red light stays on", "33" },
		{ "startGreen", "Start the light as Green (0 -> start on Red)", "0" },
		{ "inputPipe", "Name of the input pipe", "/tmp/output" },
		{ "outputPipe", "Name of the output pipe", "/tmp/input" }
	)

	SST_ELI_DOCUMENT_PORTS(
		{ "port", "Port on which to send/recv messages", {"sst.Interfaces.StringEvent"}}
	)

private:
	SST::Output output;
	SST::Link *port;
	
	int inFifo;
	int outFifo;
	
	std::string clock;
	std::string inputPipe;
	std::string outputPipe;
	int greenTime;
	int yellowTime;
	int redTime;
	int startGreen;
	char s[9];
};

#endif
