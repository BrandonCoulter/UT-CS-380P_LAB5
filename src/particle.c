#include <stdio.h>
#include <stdlib.h>
#include "particle.h"

// Function to create a particle
Particle* create_particle(int index, double x_pos, double y_pos, double mass, double x_vel, double y_vel) {
    Particle* particle = (Particle*)malloc(sizeof(Particle));
    if (particle == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    particle->index = index;
    particle->x_pos = x_pos;
    particle->y_pos = y_pos;
    particle->mass = mass;
    particle->x_vel = x_vel;
    particle->y_vel = y_vel;
    return particle;
}

// Function to destroy a particle
void destroy_particle(Particle* particle) {
    if (particle != NULL) {
        free(particle);
    }
}

// Function to print particle details
void print_particle(const Particle* particle) {
    if (particle != NULL) {
        printf("Particle %d: Position (%lf, %lf), Mass: %lf, Velocity (%lf, %lf)\n",
               particle->index, particle->x_pos, particle->y_pos, particle->mass, particle->x_vel, particle->y_vel);
    }
}


