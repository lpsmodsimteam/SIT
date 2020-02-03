#include <systemc.h>

SC_MODULE(reciprocal) {
    sc_in<sc_uint<10> > operand;
    sc_out<float> data_out;

    void div() {
        data_out.write(1 / ((float) operand.read()));
    }

    SC_CTOR(reciprocal) {
        SC_METHOD(div);
        sensitive << operand;
    }
};
