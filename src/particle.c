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
    particle->x_force = 0.0;
    particle->y_force = 0.0;
    return particle;
}

void update_particle(Particle* particle, double dt, double bounds_size) {
    // printf("Particle #%d Force components are: %lf, %lf\n", particle->index, particle->x_force, particle->y_force);
    double ax = particle->x_force / particle->mass;
    double ay = particle->y_force / particle->mass;

    // Calculate the new positions
    particle->x_pos = particle->x_pos + (particle->x_vel * dt) + (0.5 * ax * (dt * dt));
    particle->y_pos = particle->y_pos + (particle->y_vel * dt) + (0.5 * ay * (dt * dt));

    if (particle->x_pos < 0 ||
        particle->x_pos > bounds_size ||
        particle->y_pos < 0 ||
        particle->y_pos > bounds_size){
            particle->mass = -1.0;
            return;
    }

    // Calculate the new velocities
    particle->x_vel = particle->x_vel + (ax * dt);
    particle->y_vel = particle->y_vel + (ay * dt);

    return;
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


