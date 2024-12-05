#ifndef IO_H
#define IO_H

#include <stdio.h>

// Struct to represent a particle
typedef struct {
    int index;
    double x_position;
    double y_position;
    double mass;
    double x_velocity;
    double y_velocity;
} Particle;

// Function prototypes
void parse_arguments(int argc, char **argv, char **input_filename, char **output_filename, int *steps, double *theta, double *dt, int *visualization_flag);
Particle *read_input_file(const char *filename, int *num_bodies);
void write_output_file(const char *filename, Particle *particles, int num_bodies);

#endif // IO_H