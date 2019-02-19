#include "mpi.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

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

    MPI_Comm intercomm;
    MPI_Comm_get_parent(&intercomm);

    int sendbuf[3]; // redundant for worker.
    int recvbuf;

    MPI_Scatter(sendbuf, 1, MPI_INT, &recvbuf, 1, MPI_INT, 0, intercomm);
    printf("recvbuf = %d %d %d %d\n", recvbuf, world_rank, world_size, getpid());

    MPI_Finalize();
    return 0;

}