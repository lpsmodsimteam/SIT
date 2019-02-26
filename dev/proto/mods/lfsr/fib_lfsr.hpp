#include <unistd.h>
#include "systemc.h"

SC_MODULE (fib_lfsr) {

    sc_in<bool> clock;
    sc_in<bool> reset;

    sc_out<sc_uint<4>> data_out;

    sc_bv<4> din_bv;
    sc_signal<bool> fb;

    void lfsr() {

        if (reset.read()) {

            din_bv = "0001";

        } else {

            fb = (din_bv[0].to_bool() ^ din_bv[3].to_bool());
            din_bv = (din_bv.range(2, 0), fb);

        }

        data_out.write(din_bv);

    }

    // Constructor
    SC_CTOR(fib_lfsr) {
        std::cout << "INSTANTIATING FIBONACCI LFSR" << std::endl;
        SC_METHOD(lfsr);
        sensitive << clock.pos();
    }

    ~fib_lfsr() override {
        std::cout << getpid() << " DESTROYING FIBONACCI LFSR" << std::endl;
    }

}; // End of Module fib_lfsr
