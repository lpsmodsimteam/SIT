#include <systemc.h>


SC_MODULE (stoplight) {

    enum light_state {
        green, yellow, red
    };

    sc_in_clk clock;
    sc_in<bool> load;
    sc_in<bool> start_green;
    sc_in<sc_uint<6> > green_time;
    sc_in<sc_uint<2> > yellow_time;
    sc_in<sc_uint<6> > red_time;

    sc_uint<6> counter;
    sc_signal<light_state> next_state;
    sc_signal<light_state> cur_state;

    sc_out<sc_uint<2> > state;

    void get_next_state() {

        if (load.read()) {

            counter = 0;
            next_state = start_green.read() ? green : red;

        } else {

            switch (cur_state) {

                case green: {

                    if (counter == green_time) {
                        next_state = yellow;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case yellow: {

                    if (counter == yellow_time) {
                        next_state = red;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case red: {

                    if (counter == red_time) {
                        next_state = green;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

            }

        }

        cur_state = next_state;
        state.write(static_cast<int>(cur_state));

    }

    // Constructor
    SC_CTOR(stoplight) {
        std::cout << "INSTANTIATING STOPLIGHT" << std::endl;
        SC_METHOD(get_next_state)
        sensitive << clock.pos();
    }

    ~stoplight() override {
        std::cout << "DESTROYING STOPLIGHT" << std::endl;
    }

}; // End of Module stoplight
