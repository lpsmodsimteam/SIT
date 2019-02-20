#include "sysc_counter.cpp"

#include "sstsysc.hpp"
#include <systemc.h>

#include <mpi.h>

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

    /*

    // create an empty structure (null)
    json m_data_in;
    json m_data_out;


    std::string pid = to_string(getpid());
    if (write(sock_fd, pid.c_str(), pid.size()) != pid.size()) {
        perror("ERROR writing to socket");
    }

    try {

        bool flag;

        do {

            char m_buffer[BUFSIZE];

            sc_start(1, SC_NS);

            m_data_out["data_out"] = to_string(data_out);

            send_json(m_data_out, sock_fd);
            m_data_out.clear();

            m_data_in = recv_json(m_buffer, sock_fd);

            flag = m_data_in["on"].get<bool>();
            clock = (m_data_in["clock"].get<int>()) % 2;
            enable = m_data_in["enable"].get<bool>();
            reset = m_data_in["reset"].get<bool>();

            std::cout << "\033[33mCOUNTER\033[0m (pid: " << getpid() << ") -> clock: " << clock << " | enable: "
                      << m_data_in["enable"] << " | reset: " << m_data_in["reset"] << std::endl;
            m_data_in.clear();

        } while (flag);

    } catch (json::type_error &e) {

        std::cout << getpid() << " COUNTER JSON TYPE ERROR " << e.what() << m_data_in << " ON " << sock_fd << std::endl;

    }


    close(sock_fd);

    return 0;  // Terminate simulation
*/

    char sendbuf[BUFSIZE] = "cnt";
    char recvbuf[2][BUFSIZE];

    MPI_Scatter(sendbuf, BUFSIZE, MPI_CHAR, recvbuf, BUFSIZE, MPI_CHAR, 0, inter_com);
    printf("COUNTER=%s\n", *recvbuf);
    MPI_Gather(sendbuf, BUFSIZE, MPI_CHAR, recvbuf, BUFSIZE, MPI_CHAR, 0, inter_com);

    MPI_Scatter(sendbuf, BUFSIZE, MPI_CHAR, recvbuf, BUFSIZE, MPI_CHAR, 0, inter_com);
    printf("COUNTER=%s\n", *recvbuf);
    MPI_Gather(sendbuf, BUFSIZE, MPI_CHAR, recvbuf, BUFSIZE, MPI_CHAR, 0, inter_com);

//    MPI_Barrier(inter_com);
    MPI_Finalize();
    return 0;
}
