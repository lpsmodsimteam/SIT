#ifndef _intersection_H
#define _intersection_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

class intersection : public SST::Component {

public:
	intersection( SST::ComponentId_t id, SST::Params& params );
	~intersection();

	void setup();
	void finish();

	bool clockTick( SST::Cycle_t currentCycle );

	void handleLight0(SST::Event *ev);
	void handleLight1(SST::Event *ev);
	void handleCars0(SST::Event *ev);
	void handleCars1(SST::Event *ev);

	SST_ELI_REGISTER_COMPONENT(
		intersection,
		"intersection",
		"intersection",
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"Main Intersection for traffic simulation",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	SST_ELI_DOCUMENT_PARAMS(
		{ "clock", "Clock frequency or period", "1Hz" },
		{ "simDuration", "How long to run the simulation (ticks)", "200" }
	)

	SST_ELI_DOCUMENT_PORTS(
		{ "light0", "Port for connection to first traffic light", {"sst.Interfaces.StringEvent"}},
		{ "light1", "Port for connection to second traffic light", {"sst.Interfaces.StringEvent"}},
		{ "cars0", "Port on which cars are received", {"sst.Interfaces.StringEvent"}},
		{ "cars1", "Port on which cars are received", {"sst.Interfaces.StringEvent"}}
	)

private:
	SST::Output output;
	std::string clock;
	SST::Cycle_t simDuration;

	SST::Link *light0;
	SST::Link *light1;
	SST::Link *cars0;
	SST::Link *cars1;
	
	int road0;
	int road1;
	int totalCars0;
	int totalCars1;
	int backup0;
	int backup1;
        int numtics;
};

#endif
