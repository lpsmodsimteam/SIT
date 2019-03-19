#include "../modules/galois_lfsr.hpp"

#include "../../src/sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    galois_lfsr DUT("galois_lfsr");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    SignalSocket m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set("pid", getpid());
    m_signal_io.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        m_signal_io.recv();

        if (!m_signal_io.alive()) {
            break;
        }
        clock = m_signal_io.get_clock_pulse("clock");
        reset = m_signal_io.get<bool>("reset");

        // SENDING
        m_signal_io.set("data_out", data_out);
        m_signal_io.send();

    }

    return 0;

}
