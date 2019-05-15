#include "../traffic_light_fsm.hpp"
#include "../../../sstscit/sstscit.hpp"
#include "traffic_light_fsm_ports.hpp"

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> load;
    sc_signal<bool> start_green;
    sc_signal<sc_uint<6> > green_time;
    sc_signal<sc_uint<2> > yellow_time;
    sc_signal<sc_uint<6> > red_time;
    sc_signal<sc_uint<2> > state;

    // Connect the DUT
    traffic_light_fsm DUT("traffic_light_fsm");
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
    SocketSignal m_signal_io(NUM_PORTS, socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set(ports::__pid__, getpid());
    m_signal_io.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start();

        // RECEIVING
        m_signal_io.recv();

        if (!m_signal_io.alive()) {
            break;
        }
        clock = m_signal_io.get_clock_pulse(ports::__clock__);
        load = m_signal_io.get<bool>(ports::load);
        start_green = m_signal_io.get<bool>(ports::start_green);
        green_time = m_signal_io.get<int>(ports::green_time);
        yellow_time = m_signal_io.get<int>(ports::yellow_time);
        red_time = m_signal_io.get<int>(ports::red_time);

        // SENDING
        m_signal_io.set(ports::state, state);
        m_signal_io.send();

    }

    return 0;

}
