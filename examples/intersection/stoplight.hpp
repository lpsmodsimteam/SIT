#include <systemc.h>

enum light_state {
    green, yellow, red
};

SC_MODULE (stoplight) {

    sc_in_clk clock;
    sc_in<bool> load;
    sc_in<bool> start_green;
    sc_in<sc_uint<6> > green_time;
    sc_in<sc_uint<6> > yellow_time;
    sc_in<sc_uint<6> > red_time;

    sc_uint<6> counter;
    sc_signal<light_state> next_state;
    sc_signal<light_state> cur_state;

    sc_out<light_state> state;

    void get_next_state() {

        if (load.read()) {

            std::cout << "init" << std::endl;
            counter = 0;
            if (start_green.read()) {
                next_state = green;
            } else {
                next_state = red;
            }

        } else {

            switch (cur_state) {

                case red: {

                    std::cout << "red" << std::endl;
                    if (counter == red_time) {
                        next_state = green;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case green: {

                    std::cout << "green" << std::endl;
                    if (counter == green_time) {
                        next_state = yellow;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case yellow: {

                    std::cout << "yellow" << std::endl;
                    if (counter == yellow_time) {
                        next_state = red;
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

            }

        }

        cur_state = next_state;
        state.write(cur_state);

    }

    // Constructor
    SC_CTOR(stoplight) {
        std::cout << "INSTANTIATING STOPLIGHT" << std::endl;
        SC_METHOD(get_next_state);
        sensitive << next_state << cur_state << load << start_green << clock.pos();
    }

    ~stoplight() override {
        std::cout << "DESTROYING STOPLIGHT" << std::endl;
    }

}; // End of Module stoplight
