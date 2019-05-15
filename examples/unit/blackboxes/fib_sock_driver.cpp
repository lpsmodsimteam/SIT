#include "../modules/fib_lfsr.hpp"
#include "fib_ports.hpp"
#include "../../../sstscit/sstscit.hpp"

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_bv<4> > data_out;

    // Connect the DUT
    fib_lfsr DUT("FIB_LFSR");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    SocketSignal m_signal_io(FIB_NUM_PORTS, socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set(fib_ports::__pid__, getpid());
    m_signal_io.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start();

        // RECEIVING
        m_signal_io.recv();

        if (!m_signal_io.alive()) {
            break;
        }
        clock = m_signal_io.get_clock_pulse(fib_ports::__clock__);
        reset = m_signal_io.get<bool>(fib_ports::reset);

        // SENDING
        m_signal_io.set(fib_ports::data_out, data_out);
        m_signal_io.send();

    }

    return 0;

}
