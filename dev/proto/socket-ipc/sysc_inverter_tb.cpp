#include "sysc_inverter.cpp"

#include "sstsysc.hpp"
#include <systemc.h>

#include <mpi.h>

int sc_main(int argc, char *argv[]) {

    sc_signal<sc_uint<4> > data_in;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    sysc_inverter sysc_inverter("INVERTER");
    sysc_inverter.data_in(data_in);
    sysc_inverter.data_out(data_out);

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
    char *send_buf;
    char *recv_buf;

    do {

        send_buf = new char[BUFSIZE];
        recv_buf = new char[BUFSIZE];

        sc_start(1, SC_NS);

        MPI_Scatter(nullptr, BUFSIZE, MPI_CHAR, recv_buf, BUFSIZE, MPI_CHAR, 0, inter_com);
        m_data_in = json::parse(recv_buf);

        flag = m_data_in["on"].get<bool>();
        data_in = m_data_in["data_in"].get<int>();

        std::cout << "\033[33mINVERTER\033[0m (pid: " << getpid() << ") -> clock: " << sc_time_stamp() << " | data_in: "
                  << m_data_in["data_in"] << std::endl;
        m_data_in.clear();

        m_data_out["inv_out"] = to_string(data_out);

        strncpy(send_buf, to_string(m_data_out).c_str(), BUFSIZE);
        MPI_Gather(send_buf, BUFSIZE, MPI_CHAR, nullptr, BUFSIZE, MPI_CHAR, 0, inter_com);

        m_data_out.clear();

    } while (flag);

    free(send_buf);
    free(recv_buf);

    MPI_Finalize();
    return 0;

}
