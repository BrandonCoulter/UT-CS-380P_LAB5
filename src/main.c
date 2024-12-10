#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "tree.h"

#define DEFAULT_BOUNDARY_SIZE 4.0

int main(int argc, char **argv){


    // MPI Variables and Initilization
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Start the timer
    double start_time = MPI_Wtime();

    // Cmd Line Arg Initilization
    char *in_file = NULL;
    char *out_file = NULL;
    int step_count = 0, visualize = 0, dbg_print = 0;
    double theta = 0, time_step = 0;

    // Parse Command Line Arguments and Assign values
    if (rank == 0) {
        argument_parse(argc, argv, &in_file, &out_file, &step_count, &theta, &time_step, &visualize, &dbg_print);

        // Debug Printing Statements
        if (dbg_print > 0) {
            printf("\nParsed Command-Line Arguments and Settings:\n");
            printf("Process ID: %d\n", rank);
            printf("MPI Size: %d\n", size);
            printf("Input File: %s\n", in_file);
            printf("Output File: %s\n", out_file);
            printf("Number of Steps: %d\n", step_count);
            printf("Theta (MAC Threshold): %lf\n", theta);
            printf("Time Step (dt): %lf\n", time_step);
            printf("Visualization Flag: %s\n", visualize ? "Enabled" : "Disabled");
            printf("Debug Printing Flag: %s | Log level: %d\n\n", dbg_print ? "Enabled" : "Disabled", dbg_print);
        }
    }

    // Barnes-hut variables (tree and particles)
    int particle_count = 0;
    Particle* particles;

    // Default center location and boundaries
    double cen = DEFAULT_BOUNDARY_SIZE / 2;
    Boundary default_bounds = create_bounds(DEFAULT_BOUNDARY_SIZE, cen, cen);

    // Define the root node
    BHTreeNode* root_node;

    // In the main thread
    if (rank == 0) {
        // Read the input file and return a list of particles
        particles = read_input_file(in_file, &particle_count);

        // Debug Print Statements
        if (dbg_print >= 5) {
            printf("Particles: %d\n", particle_count);
            for(int i = 0; i < particle_count; i++) {
                Particle* p = &particles[i];

                // printf("Particle #%d: (%e, %e) | M: %e | (%e, %e)\n", p->index, p->x_pos, p->y_pos, p->mass, p->x_vel, p->y_vel);
                printf("Particle #%d: (%lf, %lf) | M: %e | (%e, %e)\n", p->index, p->x_pos, p->y_pos, p->mass, p->x_vel, p->y_vel);

            }
        }
    }

    // Run Barnes-Hut nbody seqientially
    if (size == 1) {
        // Debug Print statement
        if (dbg_print > 0) printf("Running seqientially\n");

        // Conduct the algorithm for n-steps 
        for (int step = 0; step < step_count; step++) {
            if (dbg_print > 0) printf("Step %d out of %d\n", step, step_count);

            // Generate the root node
            BHTreeNode* root_node = create_tree_node(default_bounds);

            if (dbg_print >= 5) print_node_data(root_node);

            // Build the BH Quadtree
            for (int p = 0; p < particle_count; p++) {
                int success = insert_node(root_node, &particles[p]);
                if (dbg_print >= 5) {
                    printf("Inserting Particle #%d\n", particles[p].index);
                    if (!success) {
                        printf("Failed to insert particle #%d\n", particles[p].index);
                    } else {
                        printf("Successfully inserted particle #%d\n\n", particles[p].index);
                    }

                }
            }

            // aggregate_data(root_node);

            // Compute the forces on each particle
            for (int p = 0; p < particle_count; p++) {
                // Reset particle force components
                particles[p].x_force = 0.0;
                particles[p].y_force = 0.0;
                
                // Compute new forces
                compute_force(root_node, &particles[p], theta);
            }

            // Update the particles based on forces from other particles
            for (int p = 0; p < particle_count; p++) {
                update_particle(&particles[p], time_step, DEFAULT_BOUNDARY_SIZE);
            }

            // Clean the tree and memory
            destroy_tree_node(root_node);

        }

    }
    // Run Barnes-Hut in Parallel with MPI
    else {
        // Broadcast input parameters to all processes
        int in_file_len = (rank == 0) ? strlen(in_file) + 1 : 0;
        MPI_Bcast(&in_file_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) in_file = (char *)malloc(in_file_len * sizeof(char));
        MPI_Bcast(in_file, in_file_len, MPI_CHAR, 0, MPI_COMM_WORLD);

        int out_file_len = (rank == 0) ? strlen(out_file) + 1 : 0;
        MPI_Bcast(&out_file_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) out_file = (char *)malloc(out_file_len * sizeof(char));
        MPI_Bcast(out_file, out_file_len, MPI_CHAR, 0, MPI_COMM_WORLD);

        MPI_Bcast(&step_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&theta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&time_step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&dbg_print, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }


    write_output_file(out_file, particles, particle_count);


    // Cleanup Memory and MPI
    if (rank != 0) {
        free(particles);
        free(in_file);
        free(out_file);
    }

    double end_time = MPI_Wtime();
    printf("%f\n", end_time - start_time);

    MPI_Finalize();

    return 0;
}