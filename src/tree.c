
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

int insert_node(BHTreeNode* node, Particle* particle) {
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

    // Update the aggregated info to include this particle
    // TODO: Implement update_node(node, particle);

    // Subdivide and redistribute if necessary
    if (!node->is_sub_divided) {
        subdivide(node);


        printf("Subdivided node, reinserting particle #%d\n", node->particle->index);
        // Redistribute the existing particle
        Particle* existing_particle = node->particle;
        node->particle = NULL; // Clear parent node's particle reference
        node->count = 0;       // Reset the count

        if (!insert_node(node->NW, existing_particle) &&
            !insert_node(node->NE, existing_particle) &&
            !insert_node(node->SW, existing_particle) &&
            !insert_node(node->SE, existing_particle)) {
            printf("Error redistributing particle #%d during subdivision.\n", existing_particle->index);
        }
        free(existing_particle); // Free the copy
    }

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
        particle->x_pos <= node->boundary.center->x_pos + node->boundary.half_size &&
        particle->y_pos >= node->boundary.center->y_pos - node->boundary.half_size &&
        particle->y_pos <= node->boundary.center->y_pos + node->boundary.half_size);
}

//TODO: COMPLETE THIS FUNCTION
// Update the mass and centor of mass
void update_node(BHTreeNode* node, Particle* particle) {
    return;
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

    // printf("Node->NW: \n");
    // print_node_data(node->NW);
    // printf("Node->NE: \n");
    // print_node_data(node->NE);
    // printf("Node->SW: \n");
    // print_node_data(node->SW);
    // printf("Node->SE: \n");
    // print_node_data(node->SE);

    node->is_sub_divided = 1;
}

//TODO: COMPLETE THIS FUNCTION
// Compute for the forces on a particle
void compute_force(BHTreeNode* node, Particle* particle, double theta, double time_step) {
    printf("Computing force for Particle #%d\n", particle->index);
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