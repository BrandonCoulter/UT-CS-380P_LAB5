#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

int main(int argc, char **argv) {
    int rank, size;

    // MPI Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Command-line argument variables
    char *input_filename = NULL;
    char *output_filename = NULL;
    int steps = 0, visualization_flag = 0;
    double theta = 0.0, dt = 0.0;

    // Master process parses arguments
    if (rank == 0) {
        parse_arguments(argc, argv, &input_filename, &output_filename, &steps, &theta, &dt, &visualization_flag);
    }

    // Broadcast input parameters to all processes
    int input_filename_length = (rank == 0) ? strlen(input_filename) + 1 : 0;
    MPI_Bcast(&input_filename_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) input_filename = (char *)malloc(input_filename_length * sizeof(char));
    MPI_Bcast(input_filename, input_filename_length, MPI_CHAR, 0, MPI_COMM_WORLD);

    int output_filename_length = (rank == 0) ? strlen(output_filename) + 1 : 0;
    MPI_Bcast(&output_filename_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) output_filename = (char *)malloc(output_filename_length * sizeof(char));
    MPI_Bcast(output_filename, output_filename_length, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&theta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int num_bodies = 0;
    Particle *particles = NULL;

    // Master process reads input file
    if (rank == 0) {
        particles = read_input_file(input_filename, &num_bodies);
    }

    // Broadcast number of bodies to all processes
    MPI_Bcast(&num_bodies, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter particles to all processes
    int local_num_bodies = num_bodies / size;
    Particle *local_particles = (Particle *)malloc(local_num_bodies * sizeof(Particle));
    MPI_Scatter(particles, local_num_bodies * sizeof(Particle), MPI_BYTE,
                local_particles, local_num_bodies * sizeof(Particle), MPI_BYTE,
                0, MPI_COMM_WORLD);

    // Main simulation loop (simplified)
    for (int step = 0; step < steps; step++) {
        // Perform calculations for local particles (this is where the Barnes-Hut algorithm will go)
        for (int i = 0; i < local_num_bodies; i++) {
            // Example: Update positions (replace with actual Barnes-Hut logic)
            local_particles[i].x_pos += local_particles[i].x_vel * dt;
            local_particles[i].y_pos += local_particles[i].y_vel * dt;
        }

        // Gather updated particles back to the master process
        MPI_Gather(local_particles, local_num_bodies * sizeof(Particle), MPI_BYTE,
                   particles, local_num_bodies * sizeof(Particle), MPI_BYTE,
                   0, MPI_COMM_WORLD);
    }

    if (rank == 0) { // Print only from the master process in MPI
        printf("Parsed Command-Line Arguments and Settings:\n");
        printf("Input File: %s\n", input_filename);
        printf("Output File: %s\n", output_filename);
        printf("Number of Steps: %d\n", steps);
        printf("Theta (MAC Threshold): %lf\n", theta);
        printf("Time Step (dt): %lf\n", dt);
        printf("Visualization Flag: %s\n", visualization_flag ? "Enabled" : "Disabled");
    }


    // Master process writes the output file
    if (rank == 0) {
        write_output_file(output_filename, particles, num_bodies);
        free(particles);
    }

    // Cleanup
    free(local_particles);
    if (rank != 0) {
        free(input_filename);
        free(output_filename);
    }

    MPI_Finalize();
    return 0;
}
