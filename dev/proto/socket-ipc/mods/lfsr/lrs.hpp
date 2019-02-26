#include <unistd.h>
#include "systemc.h"

SC_MODULE (lrs) {

    sc_in<bool> clock;
    sc_in<sc_uint<4>> in;

    sc_out<sc_uint<4>> out;

    sc_bv<4> din_bv;
    void func() {

        din_bv = in.read();
        din_bv = ("0", din_bv.range(3, 1));
        out.write(din_bv);

    }

    // Constructor
    SC_CTOR(lrs) {
        std::cout << "INSTANTIATING LOGICAL RIGHT SHIFTER" << std::endl;
        SC_METHOD(func);
        sensitive << in;
    }

    ~lrs() override {
        std::cout << getpid() << " DESTROYING LOGICAL RIGHT SHIFT" << std::endl;
    }

}; // End of Module lrs
