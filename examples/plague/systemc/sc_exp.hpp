#include <math.h>

#include <systemc.h>

SC_MODULE(sc_exp) {
    sc_in<float> operand;
    sc_out<float> data_out;

    void do_exp() {
        data_out.write(exp(operand.read()));
    }

    SC_CTOR(sc_exp) {
        SC_METHOD(do_exp);
        sensitive << operand;
    }
};
