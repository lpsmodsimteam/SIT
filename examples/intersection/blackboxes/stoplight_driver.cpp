#include "stoplight.hpp"

#include "sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> load;
    sc_signal<bool> start_green;
    sc_signal<sc_uint<6> > green_time;
    sc_signal<sc_uint<6> > yellow_time;
    sc_signal<sc_uint<6> > red_time;
    sc_signal<light_state> state;

    // Connect the DUT
    stoplight DUT("stoplight");
    DUT.clock(clock);
    DUT.load(load);
    DUT.start_green(start_green);
    DUT.green_time(green_time);
    DUT.yellow_time(yellow_time);
    DUT.red_time(red_time);
    DUT.state(state);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    SocketSignal m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set("__pid__", getpid());
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
        load = m_signal_io.get<bool>("load");
        start_green = m_signal_io.get<bool>("start_green");
        green_time = m_signal_io.get<int>("green_time");
        yellow_time = m_signal_io.get<int>("yellow_time");
        red_time = m_signal_io.get<int>("red_time");

        // SENDING
        m_signal_io.set("state", state);
        m_signal_io.send();

    }

    

    return 0;

}
