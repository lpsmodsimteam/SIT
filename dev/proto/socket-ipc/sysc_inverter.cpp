//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : inverter
// File Name : inverter.cpp
// Function : This is a 4 bit up-inverter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#include "systemc.h"

SC_MODULE (sysc_inverter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<sc_uint<4> > data_in; // 4 bit vector output of the inverter
    sc_out<sc_uint<4> > data_out; // 4 bit vector output of the inverter

    //------------Code Starts Here-------------------------
    // Below function implements actual inverter logic
    void invert() {

        data_out.write(data_in.read() ^ 1);

    } // End of function incr_count

    // Constructor for the inverter
    // Since this inverter is a positive edge triggered one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(sysc_inverter) {

        std::cout << "INSTANTIATING INVERTER" << std::endl;

        SC_METHOD(invert);
        sensitive << clock.pos();

    } // End of Constructor

    ~sysc_inverter() override {
        std::cout << getpid() << " DESTROYING INVERTER" << std::endl;
    }

}; // End of Module inverter
