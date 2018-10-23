/**
Simple 8-bit Up-Counter Model with one clock
*/

#include <sst/core/sst_config.h>
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
    // sc_main();
}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void counter::finish() {
    output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
    sc_main();
}


int counter::sc_main () {
  sc_signal<bool>   clock;
  sc_signal<bool>   reset;
  sc_signal<bool>   enable;
  sc_signal<sc_uint<4> > counter_out;
  int i = 0;
  // Connect the DUT
  _sc_counter _sc_counter("COUNTER");
    _sc_counter.clock(clock);
    _sc_counter.reset(reset);
    _sc_counter.enable(enable);
    _sc_counter.counter_out(counter_out);

  sc_start(1, SC_NS);

  // Initialize all variables
  reset = 0;       // initial value of reset
  enable = 0;      // initial value of enable
  for (i=0;i<5;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
  reset = 1;    // Assert the reset
  cout << "@" << sc_time_stamp() <<" Asserting reset\n" << '\n';
  for (i=0;i<10;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
  reset = 0;    // De-assert the reset
  cout << "@" << sc_time_stamp() <<" De-Asserting reset\n" << '\n';
  for (i=0;i<5;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
  cout << "@" << sc_time_stamp() <<" Asserting Enable\n" << '\n';
  enable = 1;  // Assert enable
  for (i=0;i<20;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
  cout << "@" << sc_time_stamp() <<" De-Asserting Enable\n" << '\n';
  enable = 0; // De-assert enable

  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << '\n';

  return 0;// Terminate simulation

 }


// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool counter::tick( SST::Cycle_t currentCycle ) {

    output.verbose(CALL_INFO, 1, 0, "Counter: %" PRIu8 "\n",
        static_cast<uint8_t>(upCounter));

    upCounter++;
    return false;
}
