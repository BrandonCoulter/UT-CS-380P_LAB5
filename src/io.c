#include "io.h"
#include <stdlib.h>
#include <string.h>

void parse_arguments(int argc, char **argv, char **input_filename, char **output_filename, int *steps, double *theta, double *dt, int *visualization_flag) {
    *visualization_flag = 0; // Default: visualization off

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            *input_filename = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            *output_filename = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            *steps = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            *theta = atof(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            *dt = atof(argv[++i]);
        } else if (strcmp(argv[i], "-V") == 0) {
            *visualization_flag = 1;
        } else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (!*input_filename || !*output_filename || !*steps || !*theta || !*dt) {
        fprintf(stderr, "Missing required arguments.\n");
        exit(EXIT_FAILURE);
    }
}

Particle *read_input_file(const char *filename, int *num_bodies) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", num_bodies);
    Particle *particles = (Particle *)malloc(*num_bodies * sizeof(Particle));
    if (!particles) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *num_bodies; i++) {
        fscanf(file, "%d %lf %lf %lf %lf %lf",
               &particles[i].index,
               &particles[i].x_pos,
               &particles[i].y_pos,
               &particles[i].mass,
               &particles[i].x_vel,
               &particles[i].y_vel);
    }

    fclose(file);
    return particles;
}

void write_output_file(const char *filename, Particle *particles, int num_bodies) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", num_bodies);
    for (int i = 0; i < num_bodies; i++) {
        fprintf(file, "%d %.6lf %.6lf %.6lf %.6lf %.6lf\n",
                particles[i].index,
                particles[i].x_pos,
                particles[i].y_pos,
                particles[i].mass,
                particles[i].x_vel,
                particles[i].y_vel);
    }

    fclose(file);
}
