#include "systemc.h"
#include "../../sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    init_MPI();

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Comm inter_com;
    MPI_Comm_get_parent(&inter_com);

    int pid = getpid();
    MPI_Gather(&pid, 1, MPI_INT, nullptr, 1, MPI_INT, 0, inter_com);
    MPI_Gather(&world_rank, 1, MPI_INT, nullptr, 1, MPI_INT, 0, inter_com);

    // create an empty structure (null)
    json _data_in;
    json _data_out;

    bool flag;
    auto *recv_buf = new char[BUFSIZE];
//
//    do {
//
//        sc_start(1, SC_NS);
//
//        receive_signals(recv_buf, inter_com, false);
//        _data_in = json::parse(recv_buf);
//
//        flag = _data_in["on"].get<bool>();
//
//        _data_in.clear();
//
//        _data_out["lsr_out"] = _sc_signal_to_int(data_out);
//
//        // _data_out.dump().c_str() is (const char *)
//        transmit_signals(_data_out.dump().c_str(), inter_com, false);
//        _data_out.clear();
//
//    } while (flag);

    _data_in = nullptr;
    _data_out = nullptr;
    delete[] recv_buf;
    recv_buf = nullptr;

    MPI_Finalize();
    return 0;

}
