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
    printf("Connecting to hello world server…\n");
    zmq::socket_t sock(context, ZMQ_REQ);
    //sock.connect("tcp://localhost:5555");
    sock.connect("ipc:///tmp/zero");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        sc_start(1, SC_NS);
        zmq::message_t request((void *) "Hello", 5, NULL);
        //        zmq::msg_init_size (&request, 5);
        //        memcpy (zmq::msg_data (&request), "Hello", 5);
        printf("Sending Hello %d…\n", request_nbr);
        sock.send(&request, 0);
        //zmq::msg_close (&request);

        zmq::message_t reply;
        sock.recv(&reply, 0);
        printf("Received World %d\n", request_nbr);
    }
    sock.close();


//    while (1) {
//
//        sc_start(1, SC_NS);
//
//        receive_signals(recv_buf, inter_com, false);
//        _data_in = json::parse(recv_buf);
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
//        // _data_out.dump().c_str() is (const char *)
//        transmit_signals(_data_out.dump().c_str(), inter_com, false);
//        _data_out.clear();
//
//    }

//    _data_in.clear();
//    _data_out.clear();
//    delete[] recv_buf;
//    _data_in = nullptr;
//    _data_out = nullptr;
//    recv_buf = nullptr;
//
//    MPI_Finalize();
    return 0;

}
