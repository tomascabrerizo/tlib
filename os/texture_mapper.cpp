#include "texture_mapper.h"


// NOTE: Gradients constructor
Gradients new_gradients(Point3D *vertices)
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
Edge new_edge(Gradients *gradients, Point3D *vertices, I32 top, I32 bottom)
{
    // TODO: Study how this really work
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
                          U32 *texture, U32 texture_width, U32 texture_height, 
                          Point3D *vertices)
{
    I32 top, middle, bottom;
    I32 middle_compare, bottom_compare;
    
    F32 y0 = vertices[0].y;
    F32 y1 = vertices[1].y;
    F32 y2 = vertices[2].y;

    // NOTE: Sort the vertices in Y
    
    if(y0 < y1)
    {
        if(y2 < y0)
        {
            top = 2;
            middle = 0;
            bottom = 1;
            middle_compare = 0;
            bottom_compare = 1;
        }
        else
        {
            top = 0;
            if(y1 < y2)
            {
                middle = 1;
                bottom = 2;
                middle_compare = 1;
                bottom_compare = 2;
            }
            else
            {
                middle = 2;
                bottom = 1;
                middle_compare = 2;
                bottom_compare = 1;
            }
        }
    }
    else
    {
        if(y2 < y1)
        {
            top = 2;
            middle = 1;
            bottom = 0;
            middle_compare = 1;
            bottom_compare = 0;
        }
        else
        {
            top = 1;
            if(y0 < y2)
            {
                middle = 0;
                bottom = 2;
                middle_compare = 3;
                bottom_compare = 2;
            }
            else
            {
                middle = 2;
                bottom = 0;
                middle_compare = 2;
                bottom_compare = 3;
            }
        }
    }

    Gradients gradients = new_gradients(vertices);
    Edge top_bottom = new_edge(&gradients, vertices, top, bottom);
    Edge top_middle = new_edge(&gradients, vertices, top, middle);
    Edge middle_bottom = new_edge(&gradients, vertices, middle, bottom);
    Edge *left, *right;
    I32 middle_is_left;

    // NOTE: Triangle is clock wise so bottom is greater tha middle
    // NOTE: Middle is right
    
    if(bottom_compare > middle_compare)
    {
        middle_is_left = 0;
        left = &top_bottom;
        right = &top_middle;
    }
    else
    {
        middle_is_left = 1;
        left = &top_middle;
        right = &top_bottom;
    }

    I32 height = top_middle.height;
    while(height--)
    {
        draw_scan_line(buffer, buffer_width, buffer_height, 
                       texture, texture_width, texture_height,
                       &gradients, left, right);
        edge_step(&top_middle);
        edge_step(&top_bottom);
    }
    
    height = middle_bottom.height;
    if(middle_is_left)
    {
        left = &middle_bottom;
        right = &top_bottom;
    }
    else
    {
        left = &top_bottom;
        right = &middle_bottom;
    }

    while(height--)
    {
        draw_scan_line(buffer, buffer_width, buffer_height, 
                       texture, texture_width, texture_height,
                       &gradients, left, right);
        edge_step(&middle_bottom);
        edge_step(&top_bottom);
    }

}

void draw_scan_line(U32 *buffer, U32 buffer_width, U32 buffer_height,
                    U32 *texture, U32 texture_width, U32 texture_height,
                    Gradients *gradients, Edge *left, Edge *right)
{
    // NOTE: We assume dest and texture are top down
    
    I32 x_start = ceil_f32i32(left->x);
    F32 x_prestep = x_start - left->x;

    U32 *dest_pixel = buffer + (left->y * buffer_width) + x_start;

    I32 width = ceil_f32i32(right->x) - x_start;

    F32 one_z = left->one_z + x_prestep * gradients->one_z_dx;
    F32 u_z = left->u_z + x_prestep * gradients->u_z_dx;
    F32 v_z = left->v_z + x_prestep * gradients->v_z_dx;

    if(width > 0)
    {
        while(width--)
        {
            F32 z = 1/one_z;
            I32 u = u_z * z;
            I32 v = v_z * z;

            *(dest_pixel++) = *(texture + (v * texture_width) + u);

            one_z += gradients->one_z_dx;
            u_z += gradients->u_z_dx;
            v_z += gradients->v_z_dx;
        }
    }
}
