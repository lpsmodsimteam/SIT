#ifndef _counter_H
#define _counter_H

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>

class counter : public SST::Component {

public:
	counter( SST::ComponentId_t id, SST::Params& params );
	~counter();

	void setup();
	void finish();

	bool tick( SST::Cycle_t currentCycle );

	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		counter, // class
		"counterSST", // element library
		"counter", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"Simple 8-bit Up-Counter Model with one clock",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

private:
	// local variables
	SST::Output output;
	uint8_t upCounter;

};

#endif
