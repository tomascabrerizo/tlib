#ifndef TEXTURE_MAPPER_H
#define TEXTURE_MAPPER_H

#include "tlib_types.h"

struct Point3D
{
    F32 x, y, z;
    F32 u, v;
};

// NOTE: Struct to store the gradients of the triangle to be render
// NOTE: a_b means a over b
struct Gradients
{
    F32 one_z[3];
    F32 u_z[3];
    F32 v_z[3];
    
    F32 one_z_dx, one_z_dy;
    F32 u_z_dx, u_z_dy;
    F32 v_z_dx, v_z_dy;
};

// NOTE: Gradients constructor
Gradients new_gradients(Point3D *vertices);

struct Edge
{
    F32 x, x_step; // NOTE: Fractional x and dx/dy
    I32 y, height;

    F32 one_z, one_z_step; // NOTE: 1/z and step
    F32 u_z, u_z_step; // NOTE: u/z and step
    F32 v_z, v_z_step; // NOTE: v/z and step
};

// NOTE: Edge constructor
Edge new_edge(Gradients *gradients, Point3D *vertices, I32 top, I32 bottom);
I32 edge_step(Edge *edge);

// NOTE: Texture mapper functions

void texture_map_triangle(U32 *buffer, U32 buffer_width, U32 buffer_height,
                          U32 *texture, U32 texture_width, U32 texture_height, 
                          Point3D *vertices);

void draw_scan_line(U32 *buffer, U32 buffer_width, U32 buffer_height,
                    U32 *texture, U32 texture_width, U32 texture_height,
                    Gradients *gradients, Edge *left, Edge *right);


#endif //TEXTURE_MAPPER_H
