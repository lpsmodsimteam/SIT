#include "galois_lfsr.hpp"

#include "../../sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    galois_lfsr DUT("GALOIS_LFSR");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);

    int pid = getpid();

    zmq::context_t context(1);

    //  Socket to talk to server
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("ipc:///tmp/zero");

    signal_packet _msg_in, _msg_out;
    _msg_out.data["pid"] = std::to_string(pid);  // new element inserted

    msgpack::sbuffer _sbuf;
    msgpack::packer<msgpack::sbuffer> _packer(&_sbuf);

    _packer.pack(_msg_out);

    zmq::message_t _data_in, _data_out;
    _data_out.rebuild(_sbuf.size());
    std::memcpy(_data_out.data(), _sbuf.data(), _sbuf.size());
    socket.send(_data_out);
    msgpack::unpacked _unpacker;

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        socket.recv(&_data_in);

        msgpack::unpack(_unpacker, (char *) (_data_in.data()), _data_in.size());
        _unpacker.get().convert(_msg_in);

        if (!std::stoi(_msg_in.data["on"])) {
            break;
        }
        clock = (std::stoi(_msg_in.data["clock"])) % 2;
        reset = std::stoi(_msg_in.data["reset"]);
        std::cout << "\033[33mGALOIS LFSR\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp()
                  << " | reset: " << _msg_in.data["reset"] << " -> galois_lfsr_out: " << data_out << std::endl;

        // SENDING
        _msg_out.data["galois_lfsr"] = std::to_string(_sc_signal_to_int(data_out));

        _sbuf.clear();
        _packer.pack(_msg_out);

        _data_out.rebuild(_sbuf.size());
        std::memcpy(_data_out.data(), _sbuf.data(), _sbuf.size());
        std::cout << "CHILD SEND: [galois_lfsr]=" << _msg_out.data["galois_lfsr"] << std::endl;
        socket.send(_data_out);

    }

    socket.close();

    return 0;

}
