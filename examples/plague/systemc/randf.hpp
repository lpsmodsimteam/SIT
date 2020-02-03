#include <systemc.h>
#include "rng.hpp"
#include "reciprocal.hpp"

SC_MODULE(randf) {
    sc_in_clk clock;
    sc_in<bool> en;
    sc_in<sc_uint<16> > seed;
    sc_in<sc_uint<8> > lower_limit;
    sc_in<sc_uint<10> > upper_limit;
    sc_out<float> data_out;

    rng rng_uut;
    reciprocal reciprocal_uut;

    sc_signal<sc_uint<10> > rng_data_out;

    SC_CTOR(randf) : rng_uut("rng_uut"), reciprocal_uut("reciprocal_uut") {
        rng_uut.en(en);
        rng_uut.clock(clock);
        rng_uut.seed(seed);
        rng_uut.lower_limit(lower_limit);
        rng_uut.upper_limit(upper_limit);
        rng_uut.data_out(rng_data_out);

        reciprocal_uut.operand(rng_data_out);
        reciprocal_uut.data_out(data_out);
    }
};
