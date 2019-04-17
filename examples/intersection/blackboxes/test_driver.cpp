#include "../stoplight.hpp"

#include "../../../src/sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> load;
    sc_signal<bool> start_green;
    sc_signal<sc_uint<6> > green_time;
    sc_signal<sc_uint<2> > yellow_time;
    sc_signal<sc_uint<6> > red_time;
    sc_signal<sc_uint<2> > state;

    // Connect the DUT
    stoplight DUT("stoplight");
    DUT.clock(clock);
    DUT.load(load);
    DUT.start_green(start_green);
    DUT.green_time(green_time);
    DUT.yellow_time(yellow_time);
    DUT.red_time(red_time);
    DUT.state(state);
    // ---------- SYSTEMC UUT INIT ---------- //

    start_green = true;
    load = true;
    clock = false;

    green_time = 20;
    yellow_time = 2;
    red_time = 20;
    for (int i = 0; i < 100; i++) {
        if (i == 2) {
            load = false;
        }
        clock = !clock;
        sc_start();
       std::cout << "state " << state << std::endl;
    }

    return 0;

}