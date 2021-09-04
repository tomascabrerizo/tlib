#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_render.h"

static M4F32 projection;

void GameInit(BackBuffer *buffer)
{
    F32 aspect = (F32)buffer->width / (F32)buffer->height;
    projection = PerspectiveM4F32(60, aspect, 0, 100);
}

void GameUpdateAndRender(BackBuffer *buffer, F32 dt)
{
    // NOTE: 3D complete transform from local space to screen space

    V4F32 v0 = _V4F32( 0.0f, -0.5f, 0.0f, 1.0f);
    V4F32 v1 = _V4F32( 0.5f,  0.5f, 0.0f, 1.0f);
    V4F32 v2 = _V4F32(-0.5f,  0.5f, 0.0f, 1.0f);

    // NOTE: Rotate vertices in Y
    static F32 angle = 0.0f;
    v0 = MultM4F32V4F32(RotateYM4F32(angle), v0);
    v1 = MultM4F32V4F32(RotateYM4F32(angle), v1);
    v2 = MultM4F32V4F32(RotateYM4F32(angle), v2);
    angle += 100 * dt;
    if(angle > 360) angle = 0;
    
    // NOTE: Translate vertices in Z
    v0 = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v0);
    v1 = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v1);
    v2 = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v2);

    // NOTE: Project vertices
    v0 = MultM4F32V4F32(projection, v0);
    v1 = MultM4F32V4F32(projection, v1);
    v2 = MultM4F32V4F32(projection, v2);

    FillTriangle(buffer, ToScreenSpace(buffer, v0),
                         ToScreenSpace(buffer, v1), 
                         ToScreenSpace(buffer, v2));
}
