#include "../modules/ram.hpp"

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_uint<ADDR_WIDTH> > address;
    sc_signal<bool> cs;
    sc_signal<bool> we;
    sc_signal<bool> oe;
    sc_signal<sc_uint<DATA_WIDTH> > data_in;
    sc_signal<sc_uint<DATA_WIDTH> > data_out;

    // Connect the DUT
    ram DUT("ram");
    DUT.address(address);
    DUT.cs(cs);
    DUT.we(we);
    DUT.oe(oe);
    DUT.data_in(data_in);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

//    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
//    // Initialize signal handlers
//    SocketSignal m_signal_io(GLSLFSR_NPORTS, socket(AF_UNIX, SOCK_STREAM, 0), false);
//    m_signal_io.set_addr(argv[1]);
//    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
//
//    // ---------- INITIAL HANDSHAKE ---------- //
//    m_signal_io.set(glslfsr_ports.pid, getpid());
//    m_signal_io.send();
//    // ---------- INITIAL HANDSHAKE ---------- //
//
//    while (true) {
//
//        sc_start();
//
//        // RECEIVING
//        m_signal_io.recv();
//
//        if (!m_signal_io.alive()) {
//            break;
//        }
//        clock = m_signal_io.get_clock_pulse(glslfsr_ports.clock);
//        reset = m_signal_io.get<bool>(glslfsr_ports.reset);
//
//        // SENDING
//        m_signal_io.set(glslfsr_ports.data_out, data_out);
//        m_signal_io.send();
//
//    }

    for (int i = 0; i < 10; i++) {
        sc_start();
        we = true;
        cs = true;
        address = 2*i;
        data_in = "00000101";
        std::cout << data_out << std::endl;
    }

    for (int i = 0; i < 10; i++) {
        sc_start();
        we = false;
        oe = true;
        cs = true;
        address = i;
        data_in = "00000101";
        std::cout << data_out << std::endl;
    }

    return 0;

}
