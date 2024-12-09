#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "particle.h"

// Function prototypes
void argument_parse(int argc, char **argv, char **input_filename, char **output_filename, int *steps, double *theta, double *dt, int *visualization_flag, int *print_debug_flag);
Particle *read_input_file(const char *filename, int *num_bodies);
void write_output_file(const char *filename, Particle *particles, int num_bodies);

#endif // IO_H