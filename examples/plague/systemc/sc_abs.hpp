#include <stdlib.h>

#include <systemc.h>

SC_MODULE(sc_abs) {
    sc_in<float> operand;
    sc_out<sc_uint<24> > data_out;

    void do_abs() {
        data_out.write(abs(operand.read()));
    }

    SC_CTOR(sc_abs) {
        SC_METHOD(operand);
        sensitive << operand;
    }
};
