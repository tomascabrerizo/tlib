#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_string.h"
#include "tlib_render.h"
#include "tlib_mesh.h"

static M4F32 projection;

#define BITMAP_WIDTH 32 
#define BITMAP_HEIGHT 32
static Bitmap randomBitmap;
static U32 bitmapData[BITMAP_WIDTH*BITMAP_WIDTH];
static Arena gameArena;
static Mesh boxMesh;

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
        U8 *comp = (U8 *)(randomBitmap.pixels + index);
        *comp++ = RandomF32()*255.0f;
        *comp++ = RandomF32()*255.0f;
        *comp++ = 255.0f;//RandomF32()*255.0f;
    }
    
    Memory memory = PlatformCreateMemory();
    gameArena = CreateArena(&memory);
    boxMesh = LoadObjFile(&gameArena, "data/box.obj");    
}

void GameUpdateAndRender(BackBuffer *buffer, F32 dt)
{
    // NOTE: 3D complete transform from local space to screen space
    // TODO: Create a render mesh funtions
    static F32 angle = 0.0f;
    for(U32 i = 0; i < boxMesh.vertexCount; i += 3)
    {
        Vertex v0 = boxMesh.vertex[i + 0]; 
        Vertex v1 = boxMesh.vertex[i + 1];
        Vertex v2 = boxMesh.vertex[i + 2];

        // NOTE: Rotate the box
        v0.pos = MultM4F32V4F32(RotateYM4F32(angle), v0.pos);
        v1.pos = MultM4F32V4F32(RotateYM4F32(angle), v1.pos);
        v2.pos = MultM4F32V4F32(RotateYM4F32(angle), v2.pos);
        //
        //v0.pos = MultM4F32V4F32(RotateZM4F32(angle), v0.pos);
        //v1.pos = MultM4F32V4F32(RotateZM4F32(angle), v1.pos);
        //v2.pos = MultM4F32V4F32(RotateZM4F32(angle), v2.pos);
        
        v0.pos = MultM4F32V4F32(RotateXM4F32(angle), v0.pos);
        v1.pos = MultM4F32V4F32(RotateXM4F32(angle), v1.pos);
        v2.pos = MultM4F32V4F32(RotateXM4F32(angle), v2.pos);

        // NOTE: Translate the box
        v0.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 5)), v0.pos);
        v1.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 5)), v1.pos);
        v2.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, 5)), v2.pos);

        // NOTE: Project vertices
        v0.pos = MultM4F32V4F32(projection, v0.pos);
        v1.pos = MultM4F32V4F32(projection, v1.pos);
        v2.pos = MultM4F32V4F32(projection, v2.pos);
        
        // NOTE: fille triangle
        FillTriangle(buffer, &randomBitmap, v0, v1, v2);
    }
    angle += 50 * dt;
    if(angle > 360) angle = 0;
    
    // NOTE: Render random texture in (0, 0) pos
    for(U32 y = 0; y < randomBitmap.height; ++y)
    {
        for(U32 x = 0; x < randomBitmap.width; ++x)
        {
            buffer->pixels[y*buffer->width+x] = randomBitmap.pixels[y*randomBitmap.width+x];
        }
    }
}
