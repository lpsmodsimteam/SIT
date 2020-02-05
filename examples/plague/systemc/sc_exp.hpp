#include <math.h>

#include <systemc.h>

SC_MODULE(sc_exp) {
    sc_in<float> operand;
    sc_in<float> data_out;

    void do_exp() {
        data_out.write(exp(operand.read()));
    }

    SC_CTOR(sc_exp) {
        SC_METHOD(operand);
        sensitive << operand;
    }
};
