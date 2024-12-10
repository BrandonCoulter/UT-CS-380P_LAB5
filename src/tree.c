
#include "tree.h"

Boundary create_bounds(double size, double x_pos, double y_pos) {
    Boundary* bounds = (Boundary*)malloc(sizeof(Boundary));
    Point* point = (Point*)malloc(sizeof(Point));

    point->x_pos = x_pos;
    point->y_pos = y_pos;

    bounds->center = point;
    bounds->size = size;
    bounds->half_size = size * 0.5;

    return *bounds;
}

BHTreeNode* create_tree_node (Boundary bounds) {
    BHTreeNode* node = (BHTreeNode*)malloc(sizeof(BHTreeNode));
    Point* center_mass = (Point*)malloc(sizeof(Point));

    center_mass->x_pos = 0.0;
    center_mass->y_pos = 0.0;

    // Assign default values to the new node
    node->boundary = bounds;
    node->particle = NULL;
    node->center_mass = center_mass;
    node->total_mass = 0.0;
    node->is_sub_divided = 0;
    node->capacity = 4;
    node->count = 0;

    return node;
}

// Insert nodes into the Quad tree
int insert_node(BHTreeNode* node, Particle* particle) {
    
    // This particle is lost
    if (particle->mass < 0){
        return 0;
    }

    // Check that the particle is contained within the bounds of the node
    if (!contains(node, particle)) {
        return 0; // Particle is outside this node's boundary
    }

    // Check if the node has no particle and isn't divided
    if (node->particle == NULL && !node->is_sub_divided) {
        node->particle = (Particle*)malloc(sizeof(Particle));
        *(node->particle) = *particle; // Copy particle data
        node->total_mass = particle->mass;
        node->center_mass->x_pos = particle->x_pos;
        node->center_mass->y_pos = particle->y_pos;
        node->count += 1;
        return 1;
    }

    // Subdivide and redistribute if necessary
    if (!node->is_sub_divided) {
        subdivide(node);

        // Redistribute the existing particle
        Particle* existing_particle = node->particle;
        node->particle = NULL; // Clear parent node's particle reference

        if (!insert_node(node->NW, existing_particle) &&
            !insert_node(node->NE, existing_particle) &&
            !insert_node(node->SW, existing_particle) &&
            !insert_node(node->SE, existing_particle)) {
            printf("Error redistributing particle #%d during subdivision.\n", existing_particle->index);
            return 0;
        }
        free(existing_particle); // Free the copy
    }

    // Update the total mass and center of mass before inserting
    double old_mass = node->total_mass;
    node->total_mass += particle->mass;

    // Update center of mass using the old total mass
    node->center_mass->x_pos = (node->center_mass->x_pos * old_mass + particle->x_pos * particle->mass) / node->total_mass;
    node->center_mass->y_pos = (node->center_mass->y_pos * old_mass + particle->y_pos * particle->mass) / node->total_mass;

    // Attempt to insert into one of the child nodes
    if (insert_node(node->NW, particle)) return 1;
    if (insert_node(node->NE, particle)) return 1;
    if (insert_node(node->SW, particle)) return 1;
    if (insert_node(node->SE, particle)) return 1;

    // Should not reach here
    return 0;
}


// Check that a particle is contained within the bounds of a node
int contains(BHTreeNode* node, Particle* particle) {
    return (
        particle->x_pos >= node->boundary.center->x_pos - node->boundary.half_size &&
        particle->x_pos < node->boundary.center->x_pos + node->boundary.half_size &&
        particle->y_pos >= node->boundary.center->y_pos - node->boundary.half_size &&
        particle->y_pos < node->boundary.center->y_pos + node->boundary.half_size);

}

// Update the mass and centor of mass
void update_node_data(BHTreeNode* node, Particle* particle) {
    // Get the new mass of the subtree rooted at this node
    double new_total_mass = node->total_mass + particle->mass;

    // Find the new center of mass
    node->center_mass->x_pos = (node->center_mass->x_pos * node->total_mass + particle->x_pos * particle->mass) / new_total_mass;
    node->center_mass->y_pos = (node->center_mass->y_pos * node->total_mass + particle->y_pos * particle->mass) / new_total_mass;

    // Set the new total mass for the node
    node->total_mass = new_total_mass;
    
    return;
}

void aggregate_data(BHTreeNode* node) {
    // If the node is a leaf, its CoM is the particle's position
    if (!node->is_sub_divided) {
        if (node->particle != NULL) {
            node->center_mass->x_pos = node->particle->x_pos;
            node->center_mass->y_pos = node->particle->y_pos;
            node->total_mass = node->particle->mass;
        } else {
            // No particle in this node
            node->total_mass = 0;
            node->center_mass->x_pos = 0;
            node->center_mass->y_pos = 0;
        }
        return;
    }

    // Internal node: Aggregate mass and CoM from children
    double total_mass = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    if (node->NW) {
        aggregate_data(node->NW); // Ensure child properties are up-to-date
        total_mass += node->NW->total_mass;
        weighted_x += node->NW->center_mass->x_pos * node->NW->total_mass;
        weighted_y += node->NW->center_mass->y_pos * node->NW->total_mass;
    }
    if (node->NE) {
        aggregate_data(node->NE);
        total_mass += node->NE->total_mass;
        weighted_x += node->NE->center_mass->x_pos * node->NE->total_mass;
        weighted_y += node->NE->center_mass->y_pos * node->NE->total_mass;
    }
    if (node->SW) {
        aggregate_data(node->SW);
        total_mass += node->SW->total_mass;
        weighted_x += node->SW->center_mass->x_pos * node->SW->total_mass;
        weighted_y += node->SW->center_mass->y_pos * node->SW->total_mass;
    }
    if (node->SE) {
        aggregate_data(node->SE);
        total_mass += node->SE->total_mass;
        weighted_x += node->SE->center_mass->x_pos * node->SE->total_mass;
        weighted_y += node->SE->center_mass->y_pos * node->SE->total_mass;
    }

    // Update the node's total mass and center of mass
    if (total_mass > 0) {
        node->center_mass->x_pos = weighted_x / total_mass;
        node->center_mass->y_pos = weighted_y / total_mass;
    } else {
        node->center_mass->x_pos = 0.0;
        node->center_mass->y_pos = 0.0;
    }
    node->total_mass = total_mass;
}

// Subdivide the node
void subdivide(BHTreeNode* node) {
    double sub_divided_size = node->boundary.size / 2;
    double sub_divided_center_size = sub_divided_size / 2;

    // Allocate memory for subdivided nodes
    node->NW = (BHTreeNode*)malloc(sizeof(BHTreeNode));
    node->NE = (BHTreeNode*)malloc(sizeof(BHTreeNode));
    node->SW = (BHTreeNode*)malloc(sizeof(BHTreeNode));
    node->SE = (BHTreeNode*)malloc(sizeof(BHTreeNode));

    // Create subdivided nodes and assign them to the parent
    node->NW = create_tree_node(create_bounds(sub_divided_size, node->boundary.center->x_pos - sub_divided_center_size, node->boundary.center->y_pos - sub_divided_center_size));
    node->NE = create_tree_node(create_bounds(sub_divided_size, node->boundary.center->x_pos + sub_divided_center_size, node->boundary.center->y_pos - sub_divided_center_size));
    node->SW = create_tree_node(create_bounds(sub_divided_size, node->boundary.center->x_pos - sub_divided_center_size, node->boundary.center->y_pos + sub_divided_center_size));
    node->SE = create_tree_node(create_bounds(sub_divided_size, node->boundary.center->x_pos + sub_divided_center_size, node->boundary.center->y_pos + sub_divided_center_size));

    node->is_sub_divided = 1;
}

// Compute for the forces on a particle
void compute_force(BHTreeNode* node, Particle* particle, double theta) {

    // Do not account for this lost particle
    if (particle->mass < 0){
        return;
    }

    // printf("Computing force for Particle #%d\n", particle->index);

    if (node == NULL) {
        // printf("Node is NULL\n");
        return;
    }
    if (node->count == 0){
        // printf("count is 0: Node->count == %d\n", node->count);
        return; // Node is empty or none existent
    }

    // calculate the vector components distances
    double dx = node->center_mass->x_pos - particle->x_pos;
    double dy = node->center_mass->y_pos - particle->y_pos;

    double distance = sqrt((dx * dx) + (dy * dy));

    // Ensure that the distance is no less than the RLIMIT to prevent infinite forces
    if (distance < RLIMIT) {
        distance = RLIMIT;
    }

    // If this is a leaf (I.e only one particle)
    if (!node->is_sub_divided && node->particle != NULL){
        // Skip the particle if the particle is itself
        if (node->particle == particle) {
            // printf("Particle is Particle\n");
            return; 
        }

        // Compute the forces on the body vector components
        particle->x_force += (G * node->particle->mass * particle->mass * dx) / (distance * distance * distance);
        particle->y_force += (G * node->particle->mass * particle->mass * dy) / (distance * distance * distance);

        // printf("Force Components Calculated for Particle #%d: Force: %lf, %lf\n", particle->index, particle->x_force, particle->y_force);

        return;
    }

    // Check MAC criteria
    double size = node->boundary.size;
    if ((size / distance) < theta) {
        // Compute the forces on the body vector components with the node
        particle->x_force += (G * node->total_mass * particle->mass * dx) / (distance * distance * distance);
        particle->y_force += (G * node->total_mass * particle->mass * dy) / (distance * distance * distance);

        // printf("Force Components Calculated (USING MAC) for Particle #%d: Force: %lf, %lf\n", particle->index, particle->x_force, particle->y_force);

        return;
    }

    // Recursively Compute node forces
    compute_force(node->NW, particle, theta);
    compute_force(node->NE, particle, theta);
    compute_force(node->SW, particle, theta);
    compute_force(node->SE, particle, theta);

}

// Debug print for BHTreeTypes
void print_node_data(BHTreeNode* node) {
    printf("Bounds: Size: %lf Pos: %lf, %lf\n", node->boundary.size, node->boundary.center->x_pos, node->boundary.center->y_pos);
    printf("Particle Count: %d\n", node->count);
    printf("Capacity: %d\n", node->capacity);
    printf("Center of Mass: %lf, %lf\n", node->center_mass->x_pos, node->center_mass->y_pos);
    printf("Total Mass: %lf\n", node->total_mass);
    printf("Is Subdivided: %s\n\n", node->is_sub_divided ? "True" : "False");
}

// Recursive function to free memory for the Quadtree
void destroy_tree_node (BHTreeNode* node) {
    if (!node) return;

    // Free the children nodes
    if (node->is_sub_divided) {
        destroy_tree_node(node->NW);
        destroy_tree_node(node->NE);
        destroy_tree_node(node->SW);
        destroy_tree_node(node->SE);
    }

    // Free the memory for the particle
    if (node->particle) free(node->particle);

    // Free the memory held by the node
    free(node);

}