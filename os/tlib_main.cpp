#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_render.h"

static M4F32 projection;

#define BITMAP_WIDTH 32
#define BITMAP_HEIGHT 32
static Bitmap randomBitmap;
static U32 bitmapData[BITMAP_WIDTH*BITMAP_WIDTH];

void GameInit(BackBuffer *buffer)
{
    // NOTE: Init projection matrix
    F32 aspect = (F32)buffer->width / (F32)buffer->height;
    projection = PerspectiveM4F32(60, aspect, 0, 100);

    // NOTE: Init random bitmap
    randomBitmap.width = BITMAP_WIDTH;
    randomBitmap.height = BITMAP_HEIGHT;
    randomBitmap.pixels = bitmapData;
    for(I32 index = 0; index < randomBitmap.width*randomBitmap.height; ++index)
    {
        U8 *comp = (U8*)(randomBitmap.pixels + index);
        *comp++ = RandomF32()*255.0f;
        *comp++ = RandomF32()*255.0f;
        *comp++ = RandomF32()*255.0f;
    }
}

void GameUpdateAndRender(BackBuffer *buffer, F32 dt)
{
    // NOTE: 3D complete transform from local space to screen space

    // TODO: Create a vertex class
    Vertex v0 = _Vertex( 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f);
    Vertex v1 = _Vertex( 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    Vertex v2 = _Vertex(-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    
    // NOTE: Rotate vertices in Y
    static F32 angle = 0.0f;
    v0.pos = MultM4F32V4F32(RotateYM4F32(angle), v0.pos);
    v1.pos = MultM4F32V4F32(RotateYM4F32(angle), v1.pos);
    v2.pos = MultM4F32V4F32(RotateYM4F32(angle), v2.pos);
    angle += 100 * dt;
    if(angle > 360) angle = 0;
    
    // NOTE: Translate vertices in Z
    v0.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v0.pos);
    v1.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v1.pos);
    v2.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 2)), v2.pos);

    // NOTE: Project vertices
    v0.pos = MultM4F32V4F32(projection, v0.pos);
    v1.pos = MultM4F32V4F32(projection, v1.pos);
    v2.pos = MultM4F32V4F32(projection, v2.pos);

    FillTriangle(buffer, &randomBitmap, v0, v1, v2);

    for(U32 y = 0; y < randomBitmap.height; ++y)
    {
        for(U32 x = 0; x < randomBitmap.width; ++x)
        {
            buffer->pixels[y*buffer->width+x] = randomBitmap.pixels[y*randomBitmap.width+x];
        }
    }
}
