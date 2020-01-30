#include <systemc.h>

SC_MODULE(minf) {
    sc_in<float> operand1;
    sc_in<float> operand2;
    sc_out<float> data_out;

    void do_minf() {
        data_out.write(!(operand2 < operand1) ? operand1 : operand2);
    }

    SC_CTOR(minf) {
        SC_METHOD(do_minf);
        sensitive << operand1 << operand2;
    }
};
