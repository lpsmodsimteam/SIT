#include <math.h>

#include <systemc.h>

SC_MODULE(sc_ceil) {
    sc_in<float> operand;
    sc_out<sc_uint<25> > data_out;

    void do_ceil() {
        data_out.write(ceil(operand.read()));
    }

    SC_CTOR(sc_ceil) {
        SC_METHOD(do_ceil);
        sensitive << operand;
    }
};
