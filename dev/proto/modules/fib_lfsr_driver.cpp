#include "fib_lfsr.hpp"

#include "../sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

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
    socket.connect(&argv[1][0u]);

    SignalReceiver sh_in(socket);
    SignalTransmitter sh_out(socket);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    sh_out.set("pid", getpid(), SC_UINT_T);
    sh_out.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        sh_in.recv();

        if (!sh_in.alive()) {
            break;
        }
        clock = sh_in.get_clock_pulse("clock");
        reset = sh_in.get<bool>("reset");
        std::cout << "\033[33mFIB LFSR\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp()
                  << " | reset: " << sh_in.get<bool>("reset") << " -> fib_lfsr_out: " << data_out << std::endl;

        // SENDING
        sh_out.set("fib_lfsr", data_out, SC_UINT_T);
        sh_out.send();

    }

    socket.close();

    return 0;

}
