//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : inverter
// File Name : inverter.cpp
// Function : This is a 4 bit up-inverter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#include "systemc.h"

SC_MODULE (inverter) {

    sc_in<sc_uint<4> > data_in; // 4 bit vector output of the inverter
    sc_out<sc_uint<4> > data_out; // 4 bit vector output of the inverter

    //------------Local Variables Here---------------------
    sc_bv<4> bv;

    //------------Code Starts Here-------------------------
    // Below function implements actual inverter logic
    void invert() {

        bv = ~data_in.read();
        data_out.write(bv);

    } // End of function incr_count

    // Constructor for the inverter
    // Since this inverter is a positive edge triggered one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(inverter) {

        std::cout << "INSTANTIATING INVERTER" << std::endl;

        SC_METHOD(invert);
        sensitive << data_in;

    } // End of Constructor

    ~inverter() override {
        std::cout << getpid() << " DESTROYING INVERTER" << std::endl;
    }

}; // End of Module inverter
