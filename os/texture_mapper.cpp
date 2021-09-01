#include "texture_mapper.h"


// NOTE: Gradients constructor
Gradients gradients(Point3D *vertices)
{
    Gradients result = {};

    // NOTE: a_b means a over b
    F32 one_dx = 1 / (((vertices[1].x - vertices[2].x) * 
                       (vertices[0].y - vertices[2].y)) -
                      ((vertices[0].x - vertices[2].x) *
                       (vertices[1].y - vertices[2].y)));
    F32 one_dy = -one_dx;

    for(I32 index = 0; index < 3; ++index)
    {
        F32 one_z = 1/vertices[index].z;
        result.one_z[index] = one_z;
        result.u_z[index] = one_z * vertices[index].u;
        result.v_z[index] = one_z * vertices[index].v;
    }
    
    // TODO: Study how this gradients really work

    // NOTE: Calculate all the gradients one per raster
    result.one_z_dx = one_dx * (((result.one_z[1] - result.one_z[2]) * 
                                 (vertices[0].y - vertices[2].y)) -
                                ((result.one_z[0] - result.one_z[2]) *
                                 (vertices[1].y - vertices[2].y)));
    result.one_z_dy = one_dy * (((result.one_z[1] - result.one_z[2]) * 
                                 (vertices[0].x - vertices[2].x)) -
                                ((result.one_z[0] - result.one_z[2]) *
                                 (vertices[1].x - vertices[2].x)));

    result.u_z_dx = one_dx * (((result.u_z[1] - result.u_z[2]) * 
                               (vertices[0].y - vertices[2].y)) -
                              ((result.u_z[0] - result.u_z[2]) *
                               (vertices[1].y - vertices[2].y)));
    result.u_z_dy = one_dy * (((result.u_z[1] - result.u_z[2]) * 
                               (vertices[0].x - vertices[2].x)) -
                              ((result.u_z[0] - result.u_z[2]) *
                               (vertices[1].x - vertices[2].x)));

    result.v_z_dx = one_dx * (((result.v_z[1] - result.v_z[2]) * 
                               (vertices[0].y - vertices[2].y)) -
                              ((result.v_z[0] - result.v_z[2]) *
                               (vertices[1].y - vertices[2].y)));
    result.v_z_dy = one_dy * (((result.v_z[1] - result.v_z[2]) * 
                               (vertices[0].x - vertices[2].x)) -
                              ((result.v_z[0] - result.v_z[2]) *
                               (vertices[1].x - vertices[2].x)));

    return result;
}

// NOTE: Edge constructor
Edge edge(Gradients *gradients, Point3D *vertices, I32 top, I32 bottom)
{
    Edge result = {};
    
    result.y = ceil_f32i32(vertices[top].y);
    I32 y_end = ceil_f32i32(vertices[bottom].y);
    result.height = y_end - result.y;

    F32 y_prestep = result.y - vertices[top].y;

    F32 real_height = vertices[bottom].y - vertices[top].y;
    F32 real_width = vertices[bottom].x - vertices[top].x;

    result.x = ((real_width * y_prestep) / real_height) + vertices[top].x;
    result.x_step = (real_width / real_height);
    F32 x_prestep = result.x - vertices[top].x;

    result.one_z = gradients->one_z[top] + 
                   y_prestep * gradients->one_z_dy +
                   x_prestep * gradients->one_z_dx;
    result.one_z_step = result.x_step * gradients->one_z_dx + gradients->one_z_dy;
    
    result.u_z = gradients->u_z[top] + 
                 y_prestep * gradients->u_z_dy +
                 x_prestep * gradients->u_z_dx;
    result.u_z_step = result.x_step * gradients->u_z_dx + gradients->u_z_dy;

    result.v_z = gradients->v_z[top] + 
                 y_prestep * gradients->v_z_dy +
                 x_prestep * gradients->v_z_dx;
    result.v_z_step = result.x_step * gradients->v_z_dx + gradients->v_z_dy;
    
    return result;
}

I32 edge_step(Edge *edge)
{
     edge->x += edge->x_step;
     ++edge->y;
     --edge->height;
     edge->u_z += edge->u_z_step;
     edge->v_z += edge->v_z_step;
     edge->one_z += edge->one_z_step;
     return edge->height;
}


void texture_map_triangle(U32 *buffer, U32 buffer_width, U32 buffer_height,
                          U32 *texture, U32 texture_width, U32 Texture_height, 
                          Point3D *vertices)
{
    // TODO: Not implemented!
}

void draw_scan_line(U32 *buffer, U32 buffer_width, U32 buffer_height,
                    U32 *texture, U32 texture_width, U32 Texture_height,
                    Gradients *gradients, Edge *left, Edge *right)
{
    // TODO: Not implemented!
}
