#include "systemc.h"

SC_MODULE (xor2) {

    sc_in<bool> in1, in2;

    sc_out<bool> out;

    void func() {

        out.write(in1.read() ^ in2.read());

    }

    // Constructor
    SC_CTOR(xor2) {
        SC_METHOD(func);
        sensitive << in1 << in2;
    }

    ~xor2() override {
        std::cout << getpid() << " DESTROYING XOR" << std::endl;
    }

}; // End of Module xor2
