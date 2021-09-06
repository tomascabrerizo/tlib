#ifndef TLIB_RENDER_H
#define TLIB_RENDER_H

#include "tlib_types.h"
#include "tlib_math.h"

struct BackBuffer
{
    U32 *pixels;
    U32 width;
    U32 height;
};

struct Gradients
{
    V4F32 color[3];
    V4F32 colorXStep;
    V4F32 colorYStep;
};

Gradients _Gradients(V2F32 v0, V2F32 v1, V2F32 v2, V4F32 c0, V4F32 c1, V4F32 c2);

struct Edge
{
    F32 x;
    F32 xStep;

    I32 startY;
    I32 endY;

    V4F32 color;
    V4F32 colorStep;
};

Edge _Edge(Gradients gradients, V2F32 start, V2F32 end, U32 minIndexY);
void StepEdge(Edge *edge);

struct Vertex
{
    V4F32 pos;
    V4F32 color;
};

Vertex _Vertex(F32 x, F32 y, F32 z, F32 red, F32 green, F32 blue);

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue);
void ScanLine(BackBuffer *buffer, Gradients *gradients, Edge *left, Edge *right, I32 y);
void ScanTriangle(BackBuffer *buffer, V2F32 minVert, V2F32 midVert, V2F32 maxVert, B8 handness,
                  V4F32 c0, V4F32 c1, V4F32 c2);
void FillTriangle(BackBuffer *buffer, Vertex v0, Vertex v1, Vertex v2);
V4F32 ToScreenSpace(BackBuffer *buffer, V4F32 v);

#endif //TLIB_RENDER_H
