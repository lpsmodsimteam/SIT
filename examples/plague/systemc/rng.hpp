#include <stdlib.h>

#include <systemc.h>

SC_MODULE(rng) {
    sc_in<sc_uint<16> > seed;
    sc_in<sc_uint<8> > lower_limit;
    sc_in<sc_uint<16> > upper_limit;
    sc_out<sc_uint<16> > data_out;

    /* initialize random seed: */
    void new_seed() {
        std::cout << "seeding\n";
        srand(seed.read());
    }        

    /* generate secret number between 1 and 10: */
    void generate() {
        if (lower_limit.read() < upper_limit.read()) {
            data_out.write(rand() % upper_limit.read() + lower_limit.read());
        }
    }

    SC_CTOR(rng) {
        SC_METHOD(new_seed);
        sensitive << seed;
        SC_METHOD(generate);
        sensitive << upper_limit << lower_limit;
    }
};
