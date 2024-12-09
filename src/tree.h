#ifndef TREE_H
#define TREE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "particle.h"

#define G 0.0001
#define RLIMIT 0.03

// Forward declaration of BHTreeNode
typedef struct BHTreeNode BHTreeNode;

// Point and Boundary structure and functions

typedef struct {
    double x_pos;
    double y_pos;
} Point;

typedef struct {
    double size;      // Full length of one side
    double half_size; // Half the length of one size
    Point* center;     // Center point position of the boundary 
} Boundary;

Boundary create_bounds(double size, double x_pos, double y_pos);


// Barnes-Hut Quadtree structure and functions

struct BHTreeNode
{
    Boundary boundary;   // The bounding box of a given node
    Particle* particle;  // Particles contained within the bounds of the node
    Point* center_mass;  // Center of mass of a given node
    double total_mass;   // The total mass of all contained particles
    int is_sub_divided;  // Flag to determine if its divided
    int capacity;        // Node capacity for particles
    int count;           // Current count of particles per node

    BHTreeNode* NW; // Top Left Quadrant
    BHTreeNode* NE; // Top Right Quadrant
    BHTreeNode* SW; // Bottom Left Quadrant
    BHTreeNode* SE; // Bottom Right Quadrant

};

BHTreeNode* create_tree_node(Boundary bounds);
int insert_node(BHTreeNode* node, Particle* particle);
int contains(BHTreeNode* node, Particle* particle);
void update_node_data(BHTreeNode* node, Particle* particle);
void aggregate_data(BHTreeNode* node);
void subdivide(BHTreeNode* node);
void compute_force(BHTreeNode* node, Particle* particle, double theta);
void print_node_data(BHTreeNode* node);
void destroy_tree_node(BHTreeNode* node);

#endif // TREE_H