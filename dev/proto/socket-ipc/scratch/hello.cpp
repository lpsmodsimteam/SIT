#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int np[2] = { 1, 1 };
    int errcodes[2];
    MPI_Comm parentcomm, intercomm;
    char *cmds[2] = { "./spawn_example", "./spawn_example" };
    MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };

    MPI_Init( &argc, &argv );
    MPI_Comm_get_parent( &parentcomm );
    if (parentcomm == MPI_COMM_NULL)
    {
        /* Create 2 more processes - this example must be called spawn_example.exe for this to work. */
        MPI_Comm_spawn_multiple( 2, cmds, MPI_ARGVS_NULL, np, infos, 0, MPI_COMM_WORLD, &intercomm, errcodes );
        printf("I'm the parent.\n");
    }
    else
    {
        printf("I'm the spawned.\n");
    }
    fflush(stdout);
    MPI_Finalize();
    return 0;
}