const int RM_NPORTS = 7;

const struct rm_ports_t {
    unsigned short int pid = 0;
    unsigned short int address = 1;
    unsigned short int cs = 2;
    unsigned short int we = 3;
    unsigned short int oe = 4;
    unsigned short int data_in = 5;
    unsigned short int data_out = 6;
} rm_ports;
