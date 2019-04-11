#include <systemc.h>

SC_MODULE (stoplight) {

    enum light_state {
        green, yellow, red
    };

    sc_in<bool> clock;
    sc_signal<light_state> next_state;
    sc_signal<light_state> cur_state;
    sc_out<sc_bv<4>> state;

    void get_next_state() {

        switch (cur_state) {
            case green: {
                next_state = yellow;
                state = "0001";
            }
            case yellow: {
                next_state = red;
                state = "0010";
            }
            case red: {
                next_state = green;
                state = "0011";
            }

        }

    }

    void set_state() {

        cur_state = next_state;

    }

    // Constructor
    SC_CTOR(stoplight) {
        std::cout << "INSTANTIATING STOPLIGHT" << std::endl;
        SC_METHOD(get_next_state);
        sensitive << cur_state;
        SC_METHOD(set_state);
        sensitive << clock.pos();
    }

    ~stoplight() override {
        std::cout << "DESTROYING STOPLIGHT" << std::endl;
    }

}; // End of Module stoplight
