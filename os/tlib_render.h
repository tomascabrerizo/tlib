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

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue);

struct Edge
{
    F32 x;
    F32 xStep;

    I32 startY;
    I32 endY;
};

Edge _Edge(V2F32 start, V2F32 end);
void StepEdge(Edge *edge);

void ScanLine(BackBuffer *buffer, Edge *left, Edge *right, I32 y);
void ScanTriangle(BackBuffer *buffer, V2F32 minVert, V2F32 midVert, V2F32 maxVert, B8 handness);

void FillTriangle(BackBuffer *buffer, V2F32 v0, V2F32 v1, V2F32 v2);

// TODO: Maybe this will be better in tlib_math.h
V2F32 ToScreenSpace(BackBuffer *buffer, V4F32 v);

#endif //TLIB_RENDER_H
