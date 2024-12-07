#include <stdio.h>
#include <stdlib.h>
#include "particle.h"

// Function to create a particle
Particle* create_particle(int index, double x_pos, double y_pos, double mass, double x_vel, double y_vel) {
    Particle* p = (Particle*)malloc(sizeof(Particle));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    p->index = index;
    p->x_pos = x_pos;
    p->y_pos = y_pos;
    p->mass = mass;
    p->x_vel = x_vel;
    p->y_vel = y_vel;
    return p;
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
        printf("Particle %d: Position (%.2f, %.2f), Mass: %.2f, Velocity (%.2f, %.2f)\n",
               particle->index, particle->x_pos, particle->y_pos, particle->mass, particle->x_vel, particle->y_vel);
    }
}

// Example custom function
void custom_function(Particle* particle) {
    // Implement custom functionality here
    // For example, adjust particle velocity based on some conditions
    if (particle != NULL) {
        particle->x_vel += 1.0; // Example adjustment
        particle->y_vel += 1.0; // Example adjustment
    }
}
