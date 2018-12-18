#include "systemc.h"
#include "counter.cpp"

int sc_main(int argc, char *argv[]) {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<8> > counter_out;

    // Connect the DUT
    sysc_counter sysc_counter("COUNTER");
    sysc_counter.clock(clock);
    sysc_counter.reset(reset);
    sysc_counter.enable(enable);
    sysc_counter.counter_out(counter_out);

    sc_start(1, SC_NS);

    // Initialize all variables
    reset = 0;       // initial value of reset
    enable = 0;      // initial value of enable
    for (int i = 0; i < 5; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    reset = 1;    // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n\n";
    for (int i = 0; i < 10; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    reset = 0;    // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n\n";
    for (int i = 0; i < 5; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    cout << "@" << sc_time_stamp() << " Asserting Enable\n\n";
    enable = 1;  // Assert enable
    for (int i = 0; i < 260; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    cout << "@" << sc_time_stamp() << " De-Asserting Enable\n\n";
    enable = 0; // De-assert enable

    cout << "@" << sc_time_stamp() << " Terminating simulation\n\n";

    return 0;// Terminate simulation

}

int main(int argc, char *argv[]) {

    // directly invoke inner function
    sc_core::sc_elab_and_sim(argc, argv);

    // invoke wrapper - both can't be called
    // sc_main(argc, argv);

    return EXIT_SUCCESS;

}
