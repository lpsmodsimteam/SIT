#include <stdlib.h>

#include <systemc.h>

SC_MODULE(abs) {
    sc_in<float> operand;
    sc_out<sc_uint<33> > data_out;

    void do_abs() {
        data_out.write(abs(operand.read()));
    }

    SC_CTOR(abs) {
        SC_METHOD(operand);
        sensitive << operand;
    }
};
