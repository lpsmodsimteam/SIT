#include <stdlib.h>

#include <systemc.h>

SC_MODULE(rng) {
    sc_in_clk clock;
    sc_in<bool> en;
    sc_in<sc_uint<16> > seed;
    sc_in<sc_uint<8> > lower_limit;
    sc_in<sc_uint<10> > upper_limit;
    sc_out<sc_uint<10> > data_out;

    /* initialize random seed: */
    void new_seed() {
        std::cout << "SEED: " << seed.read() << '\n';
        srand(seed.read());
    }

    void generate() {
        if ((lower_limit.read() < upper_limit.read()) && en.read()) {
            data_out.write(rand() % (upper_limit.read() - lower_limit.read() + 1));
        }
    }

    SC_CTOR(rng) {
        SC_METHOD(new_seed);
        SC_METHOD(generate);
        sensitive << clock;
    }
};
