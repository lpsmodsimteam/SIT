#include "minf.hpp"

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<float> operand1;
    sc_signal<float> operand2;
    sc_signal<float> data_out;

    // Connect the DUT
    minf DUT("minf");
    DUT.operand1(operand1);
    DUT.operand2(operand2);
    DUT.data_out(data_out);

    operand1 = 0.99;
    operand2 = 66.98;

    // SENDING
    sc_start();

    std::cout << data_out << '\n';

    return 0;

}
