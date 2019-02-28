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

    auto *recv_buf = new char[BUFSIZE];

    zmq::context_t context(1);

    //  Socket to talk to server
    zmq::socket_t sock(context, ZMQ_REQ);
    //sock.connect("tcp://localhost:5555");
    sock.connect("ipc:///tmp/zero");

    // HANDSHAKING
    zmq::message_t request((void *) "HELLO", 5, nullptr);
    printf("Sending request from client ...\n");
    sock.send(request);

//    zmq::message_t reply;
//    sock.recv(&reply, 0);
//    printf("Received acknowledgement from server: %s\n", (char *) reply.data());
//
//    printf("Sending request again from client ...\n");
//    sock.send(request);

    // create an empty structure (null)
    json _data_in;
    json _data_out;

    while (1) {

        sc_start(1, SC_NS);

        zmq::message_t reply;
        sock.recv(&reply, 0);
        std::cout << "CHILD RECEIVING " << reply.data() << std::endl;
//        _data_in = json::parse(std::string((char *) reply.data(), reply.size()));
//
//        if (!_data_in["on"].get<bool>()) {
//            break;
//        }
//        clock = (_data_in["clock"].get<int>()) % 2;
//        reset = _data_in["reset"].get<bool>();
//
//        std::cout << "\033[33mGALOIS LFSR\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp()
//                  << " | reset: " << _data_in["reset"] << " -> galois_lfsr_out: " << data_out << std::endl;
//        _data_in.clear();
//
//        _data_out["galois_lfsr"] = _sc_signal_to_int(data_out);
//
//        char *s = &(_data_out.dump())[0u];

        std::cout << "CHILD SENDING " << _data_out.dump() << std::endl;
        zmq::message_t request((void *) "HELLO2", BUFSIZE, nullptr);
        sock.send(request, 0);
        _data_out.clear();

    }

    sock.close();
    _data_in.clear();
    _data_out.clear();
    delete[] recv_buf;
    _data_in = nullptr;
    _data_out = nullptr;
    recv_buf = nullptr;

    return 0;

}
