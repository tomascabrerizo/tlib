#ifndef TLIB_RENDER_H
#define TLIB_RENDER_H

#include "tlib_types.h"
#include "tlib_math.h"

typedef struct
{
    U32 *pixels;
    U32 width;
    U32 height;
} BackBuffer;

typedef struct
{
    U32 *pixels;
    U32 width;
    U32 height;
} Bitmap;

// TODO: Maybe have Vertex and screenVertex
typedef struct
{
    V4F32 pos;
    V4F32 color;
    V2F32 texCoord;
} Vertex;

Vertex _Vertex(F32 x, F32 y, F32 z, F32 red, F32 green, F32 blue, F32 u, F32 v);
typedef struct {
    // NOTE: Color gradients
    V4F32 color[3];
    V4F32 colorXStep;
    V4F32 colorYStep;
    // NOTE: TexCoord gradients
    // TODO: Maybe call texCoordUStep and texCoordVStep
    V2F32 texCoord[3];
    V2F32 texCoordXStep;
    V2F32 texCoordYStep;

    F32 oneOverZ[3];
    F32 oneOverZXStep;
    F32 oneOverZYStep;
} Gradients;

Gradients _Gradients(Vertex v0, Vertex v1, Vertex v2);

typedef struct
{
    F32 x;
    F32 xStep;

    I32 startY;
    I32 endY;
    
    // NOTE: Edge color
    V4F32 color;
    V4F32 colorStep;
    // NOTE Edge texCoord
    V2F32 texCoord;
    V2F32 texCoordStep;

    F32 oneOverZ;
    F32 oneOverZStep;
} Edge;

Edge _Edge(Gradients gradients, Vertex start, Vertex end, U32 minIndexY);
void StepEdge(Edge *edge);

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue);
void CopyPixelFromBitmap(BackBuffer *buffer, Bitmap *bitmap, U32 x, U32 y, U32 srcX, U32 srcY);

V4F32 ToScreenSpace(BackBuffer *buffer, V4F32 v);

void ScanLine(BackBuffer *buffer, Bitmap *bitmap, Gradients *gradients, Edge *left, Edge *right, I32 y);
void ScanTriangle(BackBuffer *buffer, Bitmap *bitmap,
                  Vertex minVert, Vertex midVert, Vertex maxVert, B8 handness);

void FillTriangle(BackBuffer *buffer, Bitmap *bitmap, Vertex v0, Vertex v1, Vertex v2);

#endif //TLIB_RENDER_H
