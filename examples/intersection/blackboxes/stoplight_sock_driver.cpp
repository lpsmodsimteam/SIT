#include "../stoplight.hpp"

#include "../../../src/sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> load;
    sc_signal<bool> start_green;
    sc_signal<sc_bv<2> > state;

    // Connect the DUT
    stoplight DUT("STOPLIGHT");
    DUT.clock(clock);
    DUT.load(load);
    DUT.start_green(start_green);
    DUT.state(state);
    // ---------- SYSTEMC UUT INIT ---------- //

    start_green = true;
    load = true;
    clock = false;
    for (int i = 0; i < 100; i++) {
        if (i == 2) {
            load = false;
        }
        clock = !clock;
        sc_start(1, SC_NS);
//        std::cout << "state " << state << std::endl;
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
