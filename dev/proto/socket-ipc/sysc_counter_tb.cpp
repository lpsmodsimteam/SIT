#include "sysc_counter.cpp"

#include "sstsysc.hpp"
#include <systemc.h>

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

    int done_already;
    MPI_Initialized(&done_already);
    if (!done_already) {
        MPI_Init(nullptr, nullptr);
    }

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Comm inter_com;
    MPI_Comm_get_parent(&inter_com);

    int pid = getpid();
    MPI_Gather(&pid, 1, MPI_INT, nullptr, 1, MPI_INT, 0, inter_com);
    MPI_Gather(&world_rank, 1, MPI_INT, nullptr, 1, MPI_INT, 0, inter_com);

    // create an empty structure (null)
    json m_data_in;
    json m_data_out;

    bool flag;
    auto *send_buf = new std::string;
    auto *recv_buf = new char[BUFSIZE];

    do {

        sc_start(1, SC_NS);

        MPI_Scatter(nullptr, BUFSIZE, MPI_CHAR, recv_buf, BUFSIZE, MPI_CHAR, 0, inter_com);
        m_data_in = json::parse(recv_buf);

        flag = m_data_in["on"].get<bool>();
        clock = (m_data_in["clock"].get<int>()) % 2;
        enable = m_data_in["enable"].get<bool>();
        reset = m_data_in["reset"].get<bool>();

        std::cout << "\033[33mCOUNTER\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp() << " | enable: "
                  << m_data_in["enable"] << " | reset: " << m_data_in["reset"] << std::endl;
        m_data_in.clear();

        m_data_out["cnt_out"] = std::stoi(to_string(data_out));

        *send_buf = m_data_out.dump();
        // m_data_out.dump().c_str() is (const char *)
        MPI_Gather(send_buf->c_str(), BUFSIZE, MPI_CHAR, nullptr, BUFSIZE, MPI_CHAR, 0, inter_com);
        m_data_out.clear();

    } while (flag);

//    free(send_buf);
    send_buf = nullptr;
    free(recv_buf);

    MPI_Finalize();
    return 0;

}
