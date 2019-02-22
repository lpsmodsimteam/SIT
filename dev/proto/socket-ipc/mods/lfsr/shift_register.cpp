#include "systemc.h"

SC_MODULE (shift_register) {

    sc_in<bool> clock;
    sc_in<sc_uint<4> > data_in;

    sc_out<sc_uint<4> > data_out;

    void shift() {

        sc_bv<4> t_r = data_in.read();

        std::cout << "AY YO: " << data_in << " -> " << t_r;
        t_r[3] = t_r[2];
        t_r[2] = t_r[1];
        t_r[1] = t_r[0];
        t_r[0] = 0;
        std::cout << " -> " << t_r << std::endl;
        data_out.write(t_r);
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
