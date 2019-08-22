#include "../../ram.hpp"
#include "../../blackboxes/ram_ports.hpp"
#include "../../../../ssti/ssti.hpp"

#include <bitset>

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_bv<ADDR_WIDTH> > address;
    sc_signal<bool> cs;
    sc_signal<bool> we;
    sc_signal<bool> oe;
    sc_signal<sc_bv<DATA_WIDTH> > data_in;
    sc_signal<sc_bv<DATA_WIDTH> > data_out;

    // Connect the DUT
    ram DUT("ram");
    DUT.address(address);
    DUT.cs(cs);
    DUT.we(we);
    DUT.oe(oe);
    DUT.data_in(data_in);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver m_signal_i(RM_NPORTS, socket);
    ZMQTransmitter m_signal_o(RM_NPORTS, socket);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_o.set(rm_ports.pid, getpid());
    m_signal_o.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start();

        // RECEIVING
        m_signal_i.recv();

        if (!m_signal_i.alive()) {
            break;
        }
        address = std::bitset<ADDR_WIDTH>(m_signal_i.get(rm_ports.address)).to_ulong();
        cs = m_signal_i.get<bool>(rm_ports.cs);
        we = m_signal_i.get<bool>(rm_ports.we);
        oe = m_signal_i.get<bool>(rm_ports.oe);
        data_in = std::bitset<DATA_WIDTH>(m_signal_i.get(rm_ports.data_in)).to_ulong();

        // SENDING
        m_signal_o.set(rm_ports.data_out, data_out);
        m_signal_o.send();

    }

    socket.close();

    return 0;

}
