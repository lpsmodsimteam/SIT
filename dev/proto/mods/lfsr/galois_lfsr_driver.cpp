#include "galois_lfsr.hpp"

#include "../../sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    galois_lfsr DUT("GALOIS_LFSR");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    zmq::context_t context(1);

    //  Socket to talk to server
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("ipc:///tmp/zero");

    signal_packet _data_in, _data_out;
    zmq::message_t buf_in, buf_out;
    msgpack::sbuffer _sbuf;
    msgpack::packer<msgpack::sbuffer> _packer(&_sbuf);
    msgpack::unpacked _unpacker;
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    _data_out.data["pid"] = std::to_string(getpid());  // new element inserted
    send_sigs(socket, _packer, _sbuf, buf_out, _data_out);
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        recv_sigs(socket, _unpacker, buf_in, _data_in);

        if (!std::stoi(_data_in.data["on"])) {
            break;
        }
        clock = (std::stoi(_data_in.data["clock"])) % 2;
        reset = std::stoi(_data_in.data["reset"]);
        std::cout << "\033[33mGALOIS LFSR\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp()
                  << " | reset: " << _data_in.data["reset"] << " -> galois_lfsr_out: " << data_out << std::endl;

        // SENDING
        _data_out.data["galois_lfsr"] = std::to_string(_sc_signal_to_int(data_out));
        send_sigs(socket, _packer, _sbuf, buf_out, _data_out);

    }

    socket.close();

    return 0;

}
