#include <systemc.h>

SC_MODULE(sc_div) {
    sc_in<sc_uint<32>> op1;
    sc_in<sc_uint<32>> op2;
    sc_out<float> data_out;

    void do_div() {
        data_out.write(op1.read() / op2.read());
    }

    SC_CTOR(sc_div) {
        SC_METHOD(do_div);
        sensitive << op1 << op2;
    }
};
