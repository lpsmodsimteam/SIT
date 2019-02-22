#include "sysc_counter.cpp"

#include "sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    sysc_counter sysc_counter("COUNTER");
    sysc_counter.clock(clock);
    sysc_counter.reset(reset);
    sysc_counter.enable(enable);
    sysc_counter.data_out(data_out);

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
    json m_data_in;
    json m_data_out;

    bool flag;
    auto *recv_buf = new char[BUFSIZE];

    do {

        sc_start(1, SC_NS);

        receive_signals(recv_buf, inter_com, false);
        m_data_in = json::parse(recv_buf);

        flag = m_data_in["on"].get<bool>();
        clock = (m_data_in["clock"].get<int>()) % 2;
        enable = m_data_in["enable"].get<bool>();
        reset = m_data_in["reset"].get<bool>();

        std::cout << "\033[33mCOUNTER\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp() << " | enable: "
                  << m_data_in["enable"] << " | reset: " << m_data_in["reset"] << std::endl;
        m_data_in.clear();

        m_data_out["cnt_out"] = _sc_signal_to_int(data_out);

        // m_data_out.dump().c_str() is (const char *)
        transmit_signals(m_data_out.dump().c_str(), inter_com, false);
        m_data_out.clear();

    } while (flag);

    delete[] recv_buf;
    recv_buf = nullptr;

    MPI_Finalize();
    return 0;

}