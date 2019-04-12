#include <systemc.h>

SC_MODULE (stoplight) {

    enum light_state {
        green, yellow, red
    };

    sc_in_clk clock;
    sc_in<bool> load;
    sc_in<bool> start_green;

    sc_uint<2> counter;
    sc_signal<light_state> next_state;
    sc_signal<light_state> cur_state;

    sc_out<sc_bv<2> > state;

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
                    if (counter == 2) {
                        next_state = green;
                        state = "01";
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case green: {

                    std::cout << "green" << std::endl;
                    if (counter == 2) {
                        next_state = yellow;
                        state = "10";
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

                case yellow: {

                    std::cout << "yellow" << std::endl;
                    if (counter == 2) {
                        next_state = red;
                        state = "11";
                        counter = 0;
                    } else {
                        counter++;
                    }
                    break;

                }

            }

        }

        cur_state = next_state;

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
