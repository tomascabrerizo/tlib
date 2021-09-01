#include <math.h>
#include "texture_mapper.h"

I32 edge_step(Edge *edge)
{
     edge->x += edge->xstep;
     ++edge->y;
     --edge->height;
     edge->u_z += edge->u_z_step;
     edge->v_z += edge->v_z_step;
     edge->one_z += edge->one_z_step;
     return edge->height;
}
