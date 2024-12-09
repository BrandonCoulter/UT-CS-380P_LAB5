
#include "io.h"

void argument_parse(int argc, char **argv, char **in_file_name, char **out_file_name, int *steps, double *theta, double *time_step, int *visualization_flag, int *print_debug_flag) {
    *visualization_flag = 0; // Default: visualization off
    *print_debug_flag = 0;   // Default: output debug statements off
 
    // Loop through CL arguments and parse them accordingly 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            *in_file_name = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            *out_file_name = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            *steps = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            *theta = atof(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            *time_step = atof(argv[++i]);
        } else if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
            *print_debug_flag = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-V") == 0) {
            *visualization_flag = 1;
        } else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (!*in_file_name){
        fprintf(stderr, "Missing required argument: -i <input file name>\n");
        exit(EXIT_FAILURE);
    } else if (!*out_file_name){
        fprintf(stderr, "Missing required argument: -o <output file name>\n");
        exit(EXIT_FAILURE);
    } else if (!*steps){
        fprintf(stderr, "Missing required argument: -s <number of steps>\n");
        exit(EXIT_FAILURE);
    } else if (!*theta){
        fprintf(stderr, "Missing required argument: -t <theta value>\n");
        exit(EXIT_FAILURE);
    } else if (!*time_step){
        fprintf(stderr, "Missing required argument: -0 <output file name>\n");
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
        fprintf(file, "%d %lf %lf %lf %lf %lf\n",
                particles[i].index,
                particles[i].x_pos,
                particles[i].y_pos,
                particles[i].mass,
                particles[i].x_vel,
                particles[i].y_vel);
    }

    fclose(file);
}
