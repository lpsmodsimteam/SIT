#include "../stoplight.hpp"

#include "../../../src/sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<sc_bv<4> > state;

    // Connect the DUT
    stoplight DUT("FIB_LFSR");
    DUT.clock(clock);
    DUT.state(state);
    // ---------- SYSTEMC UUT INIT ---------- //

    for (int i = 0; i < 4; i++) {
        sc_start(1, SC_NS);
        std::cout << "state " << state << std::endl;
    }

//    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
//    SocketSignal m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0), false);
//    m_signal_io.set_addr(argv[1]);
//    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
//
//    // ---------- INITIAL HANDSHAKE ---------- //
//    m_signal_io.set("__pid__", getpid());
//    m_signal_io.send();
//    // ---------- INITIAL HANDSHAKE ---------- //
//
//    while (true) {
//
//        sc_start(1, SC_NS);
//
//        // RECEIVING
//        m_signal_io.recv();
//
//        if (!m_signal_io.alive()) {
//            break;
//        }
//        clock = m_signal_io.get_clock_pulse("clock");
//        reset = m_signal_io.get<bool>("reset");
//
//        // SENDING
//        m_signal_io.set("data_out", data_out);
//        m_signal_io.send();
//
//    }

    return 0;

}
