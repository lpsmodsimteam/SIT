#include "mpi.h"

int main(int argc, char *argv[]) {

    int np[2] = {1, 1};
    int errcodes[2];
    MPI_Comm parentcomm, intercomm;
    char *cmds[2] = {&m_sysc_inverter[0u], &m_sysc_counter[0u]};
    MPI_Info infos[2] = {MPI_INFO_NULL, MPI_INFO_NULL};

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

//    MPI_Comm_spawn("./worker", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm,
//                   MPI_ERRCODES_IGNORE);
    MPI_Comm_spawn_multiple(1, cmds, MPI_ARGVS_NULL, np, infos, 0, MPI_COMM_SELF, &intercomm, errcodes);

    int sendbuf[3] = {3, 5, 7};
    int recvbuf; // redundant for master.

    MPI_Scatter(sendbuf, 1, MPI_INT, &recvbuf, 1, MPI_INT, MPI_ROOT, intercomm);
    MPI_Comm_spawn("./worker1", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm,
                   MPI_ERRCODES_IGNORE);

    MPI_Finalize();
    return 0;

}
