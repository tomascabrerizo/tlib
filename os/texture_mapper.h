#ifndef TEXTURE_MAPPER_H
#define TEXTURE_MAPPER_H

#include "tlib_types.h"

struct Point3D
{
    F32 x, y, z;
    F32 u, v;
};

void texture_map_triangle(U32 *buffer, U32 buffer_width, U32 buffer_height,
                          U32 *texture, U32 texture_width, U32 Texture_height, Point3D *vertices);

//NOTE: Struct to store the gradiens of the triangle to be render
struct Gradients
{
    F32 one_z[3];
    F32 u_z[3];
    F32 v_z[3];
    
    F32 one_z_dx, one_z_dy;
    F32 u_z_dx, u_z_dy;
    F32 v_z_dx, v_z_dy;
};

struct Edge
{
    F32 x, xstep; //NOTE: Fractional x and dx/dy
    I32 y, height;

    F32 one_z, one_z_step; //NOTE: 1/z and step
    F32 u_z, u_z_step; //NOTE: u/z and step
    F32 v_z, v_z_step; //NOTE: v/z and step
};

I32 edge_step(Edge *edge);

#endif //TEXTURE_MAPPER_H
