#ifndef PARTICLE_H
#define PARTICLE_H

typedef struct {
    int index;
    double x_pos; // X position
    double y_pos; // Y position
    double mass;  // Mass
    double x_vel; // X velocity
    double y_vel; // Y velocity
    double x_force;
    double y_force;
} Particle;

// Functions related to particle creation, destruction, and modification
Particle* create_particle(int index, double x_pos, double y_pos, double mass, double x_vel, double y_vel);
void update_particle(Particle* particle, double dt, double bounds_size);
void destroy_particle(Particle* particle);

// Debug functions
void print_particle(const Particle* particle);





#endif // PARTICLE_H
