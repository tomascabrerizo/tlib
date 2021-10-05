#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_string.h"
#include "tlib_render.h"
#include "tlib_mesh.h"
#include "tlib_darray.h"

#define NUMBER_OF_STARS 512

typedef struct
{
    V3F32 pos[NUMBER_OF_STARS];
    V3F32 color[NUMBER_OF_STARS];
} StarField;

static M4F32 projection;

#define BITMAP_WIDTH 32 
#define BITMAP_HEIGHT 32
static Bitmap randomBitmap;
static U32 bitmapData[BITMAP_WIDTH*BITMAP_WIDTH];
static Arena gameArena;
static Mesh testMesh;
static StarField gameStarField;

void StarFieldInit(StarField *starField)
{
    for(I32 index = 0; index < ArrayCount(starField->pos); ++index)
    {
        // NOTE: The coordinates are normallize to -1 to 1
        starField->pos[index].x = (RandomF32() - 0.5f) * 2;
        starField->pos[index].y = (RandomF32() - 0.5f) * 2;
        starField->pos[index].z = (RandomF32() - 0.5f) * 2;
        
        // NOTE: Color are from 0 to 1 
        starField->color[index].x = RandomF32();
        starField->color[index].y = RandomF32();
        starField->color[index].z = RandomF32();
    }
}

void StarFieldUpdateAndRender(BackBuffer *buffer, F32 dt, StarField *starField)
{
    // NOTE: Project coordinate to the buffer width and height
    U32 halfBufferWidht = (U32)(0.5f*buffer->width);
    U32 halfBufferHeight = (U32)(0.5f*buffer->height);
    for(I32 index = 0; index < ArrayCount(starField->pos); ++index)
    {
        // NOTE: Update Star
        starField->pos[index].z -= dt;

        // NOTE: Render Star
        if(starField->pos[index].z <= 0)
        {
            starField->pos[index].x = (RandomF32() - 0.5f) * 2;
            starField->pos[index].y = (RandomF32() - 0.5f) * 2;
            starField->pos[index].z = (RandomF32() - 0.5f) * 2;
        }
        else
        {
            // NOTE: Project Star into screen space
            F32 z = starField->pos[index].z;
            I32 screenX = (I32)((starField->pos[index].x/z * halfBufferWidht) + halfBufferWidht);
            I32 screenY = (I32)((starField->pos[index].y/z * halfBufferHeight) + halfBufferHeight);
            
            if(screenX < 0 || screenX >= (I32)buffer->width || 
               screenY < 0 || screenY >= (I32)buffer->height)
            {
                starField->pos[index].x = (RandomF32() - 0.5f) * 2;
                starField->pos[index].y = (RandomF32() - 0.5f) * 2;
                starField->pos[index].z = (RandomF32() - 0.5f) * 2;
            }
            else
            {
                // NOTE: Map color from 0 to 1 to 127 to 255
                U8 red =   (U8)(starField->color[index].x * 127 + 128);
                U8 green = (U8)(starField->color[index].y * 127 + 128);
                U8 blue =  (U8)(starField->color[index].z * 127 + 128);
                DrawPixel(buffer, screenX, screenY, red, green, blue); 
            }
        }
    }
}


void ClearBackBuffer(BackBuffer *buffer, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    for(U32 index = 0; index < buffer->width*buffer->height; ++index)
    {
        buffer->pixels[index] = color;
    }
}

void ClearZBuffer(BackBuffer *buffer)
{
    for(U32 i = 0; i < buffer->width*buffer->height; ++i)
    {
        // NOTE: Should be a number greater than 1
        buffer->zBuffer[i] = 10.0f;
    }
}

// TODO: Move this into the math file
V3F32 NormV3F32(V3F32 a)
{
    return DivV3F32(a, LenghtV3F32(a));
}

void GameInit(BackBuffer *buffer)
{
    // NOTE: Init projection matrix
    F32 aspect = (F32)buffer->width / (F32)buffer->height;
    projection = PerspectiveM4F32(60, aspect, 0.1f, 1000.0f);

    // NOTE: Init random bitmap
    randomBitmap.width = BITMAP_WIDTH;
    randomBitmap.height = BITMAP_HEIGHT;
    randomBitmap.pixels = bitmapData;
    for(U32 index = 0; index < randomBitmap.width*randomBitmap.height; ++index)
    {
        U8 *comp = (U8 *)(randomBitmap.pixels + index);
        *comp++ = (U8)(RandomF32()*255.0f);
        *comp++ = (U8)(RandomF32()*255.0f);
        *comp++ = (U8)255.0f;//(RandomF32()*255.0f);
    }
    
    Memory memory = PlatformCreateMemory();
    gameArena = CreateArena(&memory);
    testMesh = LoadObjFile(&gameArena, "data/monkey.obj");   
    
    StarFieldInit(&gameStarField);
}

void GameUpdateAndRender(BackBuffer *buffer, F32 dt)
{
    // NOTE: Clear the render buffers each frame
    ClearBackBuffer(buffer, 0x00, 0x00, 0x00);
    ClearZBuffer(buffer);

    StarFieldUpdateAndRender(buffer, dt, &gameStarField);

    // NOTE: 3D complete transform from local space to screen space
    // TODO: Create a render mesh funtions
    static F32 angle = 0.0f;
    for(U32 i = 0; i < testMesh.vertexCount; i += 3)
    {
        Vertex v0 = testMesh.vertex[i + 0]; 
        Vertex v1 = testMesh.vertex[i + 1];
        Vertex v2 = testMesh.vertex[i + 2];

        // NOTE: Rotate the box
        v0.pos = MultM4F32V4F32(RotateYM4F32(angle), v0.pos);
        v1.pos = MultM4F32V4F32(RotateYM4F32(angle), v1.pos);
        v2.pos = MultM4F32V4F32(RotateYM4F32(angle), v2.pos);
        
        //v0.pos = MultM4F32V4F32(RotateXM4F32(angle), v0.pos);
        //v1.pos = MultM4F32V4F32(RotateXM4F32(angle), v1.pos);
        //v2.pos = MultM4F32V4F32(RotateXM4F32(angle), v2.pos);

        // NOTE: Translate the box
        F32 testZ = SinF32(ToRadians(angle))*5 + 3;
        v0.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, testZ)), v0.pos);
        v1.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, testZ)), v1.pos);
        v2.pos = MultM4F32V4F32(TranslateM4F32(_V3F32(0, 0, testZ)), v2.pos);

        // NOTE: Project vertices
        v0.pos = MultM4F32V4F32(projection, v0.pos);
        v1.pos = MultM4F32V4F32(projection, v1.pos);
        v2.pos = MultM4F32V4F32(projection, v2.pos);
        
        // NOTE: fille triangle
        DrawTriangle(buffer, &randomBitmap, v0, v1, v2);
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
