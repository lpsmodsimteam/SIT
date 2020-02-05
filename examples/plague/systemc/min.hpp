#include <systemc.h>

SC_MODULE(minf) {
    sc_out<sc_uint<24> > operand1;
    sc_out<sc_uint<24> > operand2;
    sc_out<sc_uint<24> > data_out;

    void do_min() {
        data_out.write(!(operand2 < operand1) ? operand1 : operand2);
    }

    SC_CTOR(minf) {
        SC_METHOD(do_min);
        sensitive << operand1 << operand2;
    }
};
