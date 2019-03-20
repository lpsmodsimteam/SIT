#include "../modules/fib_lfsr.hpp"

#include "../../../src/sstscit.hpp"

int sc_main(int argc, char *argv[]) {

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

    ZMQReceiver m_signal_i(socket);
    ZMQTransmitter m_signal_o(socket);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_o.set("pid", getpid());
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
        std::cout << "\033[33mFIB LFSR\033[0m -> clock: "
                  << sc_time_stamp() << " | reset: " << m_signal_i.get<bool>("reset")
                  << " -> fib_lfsr_out: " << data_out << std::endl;

        // SENDING
        m_signal_o.set("data_out", data_out);
        m_signal_o.send();

    }

    socket.close();

    return 0;

}
