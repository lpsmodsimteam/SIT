#include <unistd.h>
#include "systemc.h"

SC_MODULE (galois_lfsr) {

    sc_in<bool> clock;
    sc_in<bool> reset;

    sc_out<sc_uint<4> > data_out;

    sc_bv<4> din_bv;
    sc_bv<4> mask = "1001";

    void lfsr() {

        if (reset.read()) {

            din_bv = "0001";

        } else {

            if (din_bv[0]) {

                din_bv = ("0", din_bv.range(3, 1)) ^ mask;

            } else {

                din_bv = ("0", din_bv.range(3, 1));

            }

        }

        data_out.write(din_bv);

    }

    // Constructor
    SC_CTOR(galois_lfsr) {
        std::cout << "INSTANTIATING GALOIS LFSR" << std::endl;
        SC_METHOD(lfsr);
        sensitive << clock.pos();
    }

    ~galois_lfsr() override {
        std::cout << getpid() << " DESTROYING GALOIS LFSR" << std::endl;
    }

}; // End of Module galois_lfsr
