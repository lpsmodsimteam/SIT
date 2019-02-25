#include "systemc.h"

SC_MODULE (shift_register) {

    sc_in<bool> clock;
    sc_in<bool> reset;
    sc_in<bool> feedback;

    sc_out<sc_uint<4> > data_out;
    sc_bv<4> din_bv;
    sc_bv<4> mask = "1001";

    void shift() {

        if (reset.read()) {

            din_bv = "0001";

        } else {

            if (din_bv[0]) {
                din_bv = ("0", din_bv.range(3, 1)) ^ mask;
            } else {
                din_bv = ("0", din_bv.range(3, 1));
            }

        }

        data_out.write(din_bv);

    }

    // Constructor
    SC_CTOR(shift_register) {
        SC_METHOD(shift);
        sensitive << clock.pos();
    }

    ~shift_register() override {
        std::cout << getpid() << " DESTROYING SHIFT REGISTER" << std::endl;
    }

}; // End of Module shift_register
