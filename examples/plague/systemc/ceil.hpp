#include <math.h>

#include <systemc.h>

SC_MODULE(ceil) {
    sc_in<float> operand;
    sc_out<sc_uint<33> > data_out;

    void do_ceil() {
        data_out.write(ceil(operand.read()));
    }

    SC_CTOR(ceil) {
        SC_METHOD(operand);
        sensitive << operand;
    }
};
