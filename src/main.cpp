#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the process (its ID)
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the total number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Print a message from each process
    printf("Hello World, %d of %d (%d)\n", world_rank, world_size, world_rank * world_size);

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
