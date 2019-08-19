#include "../modules/fib_lfsr.hpp"
#include "fib_lfsr_ports.hpp"
#include "../../../ssti/ssti.hpp"

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
    zmq::context_t context(1);

    //  Socket to talk to server
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    ZMQReceiver m_signal_i(FIB_NUM_PORTS, socket);
    ZMQTransmitter m_signal_o(FIB_NUM_PORTS, socket);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_o.set(fblfsr_ports.pid, getpid());
    m_signal_o.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start();

        // RECEIVING
        m_signal_i.recv();

        if (!m_signal_i.alive()) {
            break;
        }
        clock = m_signal_i.get_clock_pulse(fblfsr_ports.clock);
        reset = m_signal_i.get<bool>(fblfsr_ports.reset);

        // SENDING
        m_signal_o.set(fblfsr_ports.data_out, data_out);
        m_signal_o.send();

    }

    socket.close();

    return 0;

}
