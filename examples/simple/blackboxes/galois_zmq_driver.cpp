#include "../modules/galois_lfsr.hpp"

#include "../../../src/sstscit.hpp"

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
    // Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver m_signal_i(socket);
    ZMQTransmitter m_signal_o(socket);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_o.set("__pid__", getpid());
    m_signal_o.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        m_signal_i.recv();

        if (!m_signal_i.alive()) {
            break;
        }
        clock = m_signal_i.get_clock_pulse("clock");
        reset = m_signal_i.get<bool>("reset");

        // SENDING
        m_signal_o.set("data_out", data_out);
        m_signal_o.send();

    }

    socket.close();

    return 0;

}
