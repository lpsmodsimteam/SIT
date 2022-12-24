#include <systemc.h>

SC_MODULE(sc_sumsq) {
    sc_in<float> op1;
    sc_in<float> op2;
    sc_in<float> op3;
    sc_in<float> op4;
    sc_out<float> data_out;

    void do_mul() {
        data_out.write(op1.read() * op2.read() + op3.read() * op4.read());
    }

    SC_CTOR(sc_sumsq) {
        SC_METHOD(do_mul);
        sensitive << op1 << op2 << op3 << op4;
    }
};
