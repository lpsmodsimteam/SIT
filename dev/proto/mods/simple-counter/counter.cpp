//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : counter
// File Name : counter.cpp
// Function : This is a 4 bit up-counter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#include "systemc.h"

SC_MODULE (counter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<bool> reset;      // active high, synchronous Reset input
    sc_in<bool> enable;      // Active high enable signal for counter
    sc_out<sc_uint<4> > data_out; // 4 bit vector output of the counter

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

        }
        data_out.write(count);

    } // End of function incr_count

    // Constructor for the counter
    // Since this counter is a positive edge triggered one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(counter) {

        std::cout << "INSTANTIATING COUNTER" << std::endl;

        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();

    } // End of Constructor

    ~counter() override {
        std::cout << getpid() << " DESTROYING COUNTER" << std::endl;
    }

}; // End of Module counter
