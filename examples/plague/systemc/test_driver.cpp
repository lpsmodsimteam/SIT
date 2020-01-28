#include "reci_rand.hpp"

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_uint<16>> seed;
    sc_signal<sc_uint<8>> lower_limit;
    sc_signal<sc_uint<16>> upper_limit;
    sc_signal<float> data_out;

    // Connect the DUT
    reci_rand DUT("reci_rand");
    DUT.seed(seed);
    DUT.lower_limit(lower_limit);
    DUT.upper_limit(upper_limit);
    DUT.data_out(data_out);

    seed = 420;
    lower_limit = 2;
    upper_limit = 10000;

    // SENDING
    sc_start();

    std::cout << data_out << '\n';

    return 0;

}
