#include "sysc_inverter.cpp"

#include "sstscit.hpp"

int sc_main(int argc, char *argv[]) {

    sc_signal<sc_uint<4> > data_in;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    sysc_inverter sysc_inverter("INVERTER");
    sysc_inverter.data_in(data_in);
    sysc_inverter.data_out(data_out);

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
        data_in = m_data_in["data_in"].get<int>();

        std::cout << "\033[33mINVERTER\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp() << " | data_in: "
                  << m_data_in["data_in"] << std::endl;
        m_data_in.clear();

        m_data_out["inv_out"] = _sc_signal_to_int(data_out);

        // m_data_out.dump().c_str() is (const char *)
        transmit_signals(m_data_out.dump().c_str(), inter_com, false);

        m_data_out.clear();

    } while (flag);

    m_data_out = nullptr;
    delete[] recv_buf;
    recv_buf = nullptr;

    MPI_Finalize();
    return 0;

}
