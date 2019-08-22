#include "../../ram.hpp"
#include "ram_ports.hpp"
#include "../../../../ssti/ssti.hpp"

#include <bitset>

int sc_main(int, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_bv<8> > address;
    sc_signal<bool> cs;
    sc_signal<bool> we;
    sc_signal<bool> oe;
    sc_signal<sc_bv<8> > data_in;
    sc_signal<sc_bv<8> > data_out;

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
    // Initialize signal handlers
    SocketSignal m_signal_io(RM_NPORTS, socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set(rm_ports.pid, getpid());
    m_signal_io.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start();

        // RECEIVING
        m_signal_io.recv();

        if (!m_signal_io.alive()) {
            break;
        }
        ;
        address = std::bitset<8>(m_signal_io.get(rm_ports.address)).to_ulong();
        cs = m_signal_io.get<bool>(rm_ports.cs);
        we = m_signal_io.get<bool>(rm_ports.we);
        oe = m_signal_io.get<bool>(rm_ports.oe);
        data_in = std::bitset<8>(m_signal_io.get(rm_ports.data_in)).to_ulong();

        // SENDING
        m_signal_io.set(rm_ports.data_out, data_out);
        m_signal_io.send();

    }

    

    return 0;

}
