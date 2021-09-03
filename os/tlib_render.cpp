#include "tlib_render.h"

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    buffer->pixels[y * buffer->width + x] = color;
}

Edge _Edge(V2F32 start, V2F32 end)
{
    Edge result = {};
    result.startY = CeilF32I32(start.y);
    result.endY = CeilF32I32(end.y);
    
    F32 yDist = (end.y - start.y); 
    F32 xDist = (end.x - start.x); 
    result.xStep = xDist / yDist;
    result.x = start.x;

    return result;
}

void StepEdge(Edge *edge)
{
    edge->x += edge->xStep;
}

void ScanLine(BackBuffer *buffer, Edge *left, Edge *right, I32 y)
{
    I32 startX = CeilF32I32(left->x);
    I32 endX = CeilF32I32(right->x);

    for(I32 x = startX; x < endX; ++x)
    {
        DrawPixel(buffer, x, y, 0xFF, 0xFF, 0xFF);
    }
}

void ScanTriangle(BackBuffer *buffer, V2F32 minVert, V2F32 midVert, V2F32 maxVert, B8 handness)
{
    Edge topToBottom = _Edge(minVert, maxVert);
    Edge topToMiddle = _Edge(minVert, midVert);
    Edge middleToBottom = _Edge(midVert, maxVert);
    
    Edge *left = &topToBottom;
    Edge *right = &topToMiddle;
    if(handness)
    {
        Edge *temp = left;
        left = right;
        right = temp;
    }

    I32 startY = topToMiddle.startY;
    I32 endY = topToMiddle.endY;

    for(I32 y = startY; y < endY; ++y)
    {
        ScanLine(buffer, left, right, y);
        StepEdge(left);
        StepEdge(right);
    }
    
    left = &topToBottom;
    right = &middleToBottom;
    if(handness)
    {
        Edge *temp = left;
        left = right;
        right = temp;
    }

    startY = middleToBottom.startY;
    endY = middleToBottom.endY;

    for(I32 y = startY; y < endY; ++y)
    {
        ScanLine(buffer, left, right, y);
        StepEdge(left);
        StepEdge(right);
    }
}

void FillTriangle(BackBuffer *buffer, V2F32 v0, V2F32 v1, V2F32 v2)
{
    V2F32 minVert = v0;
    V2F32 midVert = v1;
    V2F32 maxVert = v2;

    if(midVert.y < minVert.y)
    {
        V2F32 temp = midVert;
        midVert = minVert;
        minVert = temp;
    }
    if(maxVert.y < midVert.y)
    {
        V2F32 temp = midVert;
        midVert = maxVert;
        maxVert = temp;
    }
    if(midVert.y < minVert.y)
    {
        V2F32 temp = midVert;
        midVert = minVert;
        minVert = temp;
    }

    F32 areaTimeTwo = CrossV2F32(SubV2F32(maxVert, minVert), SubV2F32(midVert, minVert));
    B8 handness = (areaTimeTwo >= 0) ? 1 : 0;
    ScanTriangle(buffer, minVert, midVert, maxVert, handness);
}
