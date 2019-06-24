const int GLSLFSR_NPORTS = 4;

const struct glslfsr_ports_t {
    unsigned short int pid = 0;
    unsigned short int clock = 1;
    unsigned short int reset = 2;
    unsigned short int data_out = 3;
} glslfsr_ports;
