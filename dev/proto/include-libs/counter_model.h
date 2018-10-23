#ifndef _counter_model_H
#define _counter_model_H

#include "systemc.h"


SC_MODULE (_sc_counter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<bool> reset;      // active high, synchronous Reset input
    sc_in<bool> enable;      // Active high enable signal for counter
    sc_out< sc_uint<4> > counter_out; // 4 bit vector output of the counter

    //------------Local Variables Here---------------------
    sc_uint<4> count;

    //------------Code Starts Here-------------------------
    // Below function implements actual counter logic
    void incr_count() {
        // At every rising edge of clock we check if reset is active
        // If active, we load the counter output with 4'b0000
        if (reset.read()) {
            count = 0;
            // If enable is active, then we increment the counter
        } else if (enable.read()) {
            count++;
            cout << "@" << sc_time_stamp() << " :: Incremented Counter "
                 << counter_out.read() << endl;
        }
        counter_out.write(count);
    } // End of function incr_count

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(_sc_counter) {
        cout << "Executing new" << endl;
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

}; // End of Module counter


#endif
