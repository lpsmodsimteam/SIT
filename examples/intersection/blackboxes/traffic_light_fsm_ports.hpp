const int TRFFCLGHTFSM_NPORTS = 8;

const struct trffclghtfsm_ports_t {
    unsigned short int pid = 0;
    unsigned short int clock = 1;
    unsigned short int load = 2;
    unsigned short int start_green = 3;
    unsigned short int green_time = 4;
    unsigned short int yellow_time = 5;
    unsigned short int red_time = 6;
    unsigned short int state = 7;
} trffclghtfsm_ports;
