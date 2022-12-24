#include <systemc.h>

SC_MODULE(sc_cacc) {
    sc_in<float> dist;
    sc_in<float> inner;
    sc_in<float> outer;
    sc_out<float> new_inner;
    sc_out<float> new_outer;

    const float radius = 1.0;
    float temp_inner = inner.read();
    float temp_outer = outer.read();

    void cond_acc() {
        if (dist < radius) {
            temp_inner += 1;
        } else {
            temp_outer += 1;
        }
        new_inner.write(temp_inner);
        new_outer.write(temp_outer);
    }

    SC_CTOR(sc_cacc) {
        SC_METHOD(cond_acc);
        sensitive << dist << inner << outer;
    }
};
