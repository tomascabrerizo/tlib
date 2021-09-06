#include "tlib_render.h"

// NOTE: Gradients functions

Gradients _Gradients(V2F32 v0, V2F32 v1, V2F32 v2, 
                     V4F32 c0, V4F32 c1, V4F32 c2)
{
    Gradients result = {};
    
    result.color[0] = c0;
    result.color[1] = c1;
    result.color[2] = c2;
    
    F32 oneOverDX = 1.0f / ((v1.x - v2.x)*(v0.y - v2.y) - (v0.x - v2.x)*(v1.y - v2.y));
    F32 oneOverDY = -oneOverDX;

    V4F32 dColorX = SubV4F32(ScaleV4F32(SubV4F32(c1, c2), (v0.y - v2.y)), 
                             ScaleV4F32(SubV4F32(c0, c2), (v1.y - v2.y)));

    V4F32 dColorY = SubV4F32(ScaleV4F32(SubV4F32(c1, c2), (v0.x - v2.x)), 
                             ScaleV4F32(SubV4F32(c0, c2), (v1.x - v2.x)));

    result.colorXStep = ScaleV4F32(dColorX, oneOverDX);
    result.colorYStep = ScaleV4F32(dColorY, oneOverDY);

    return result;
}

// NOTE: Edge functions

Edge _Edge(Gradients gradients, V2F32 start, V2F32 end, U32 minIndexY)
{
    Edge result = {};
    result.startY = CeilF32I32(start.y);
    result.endY = CeilF32I32(end.y);
    
    F32 yDist = (end.y - start.y); 
    F32 xDist = (end.x - start.x); 
    
    F32 yPreStep = result.startY - start.y;

    result.xStep = xDist / yDist;
    result.x = start.x + yPreStep * result.xStep;
    
    F32 xPreStep = result.x - start.x;
    result.color = AddV4F32(AddV4F32(gradients.color[minIndexY], ScaleV4F32(gradients.colorYStep, yPreStep)),
                            ScaleV4F32(gradients.colorXStep, xPreStep));
    
    result.colorStep = AddV4F32(gradients.colorYStep, ScaleV4F32(gradients.colorXStep, result.xStep));

    return result;
}

void StepEdge(Edge *edge)
{
    edge->x += edge->xStep;
    edge->color = AddV4F32(edge->color, edge->colorStep);
}

// NOTE: Vertex functions

Vertex _Vertex(F32 x, F32 y, F32 z, F32 red, F32 green, F32 blue)
{
    Vertex result = { _V4F32(x, y, z, 1), _V4F32(red, blue, green, 1.0f)};
    return result;
}

// NOTE: Rasterizer functions

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    buffer->pixels[y * buffer->width + x] = color;
}

void ScanLine(BackBuffer *buffer, Gradients *gradients, Edge *left, Edge *right, I32 y)
{
    I32 startX = CeilF32I32(left->x);
    I32 endX = CeilF32I32(right->x);

    F32 xPreStep = (startX - left->x);

    V4F32 minColor = AddV4F32(left->color, ScaleV4F32(gradients->colorXStep, xPreStep));
    V4F32 maxColor = AddV4F32(right->color, ScaleV4F32(gradients->colorXStep, xPreStep));
     
    F32 lerpT = 0;
    F32 lerpStep = 1.0f / (F32)(endX - startX);

    for(I32 x = startX; x < endX; ++x)
    {
        V4F32 color = ScaleV4F32(LerpV4F32(minColor, maxColor, lerpT), 255);
        DrawPixel(buffer, x, y, color.x, color.y, color.z);
        lerpT += lerpStep;
    }
}

void ScanTriangle(BackBuffer *buffer, V2F32 minVert, V2F32 midVert, V2F32 maxVert, B8 handness,
                  V4F32 c0, V4F32 c1, V4F32 c2)
{
    Gradients gradients = _Gradients(minVert, midVert, maxVert, c0, c1, c2);

    Edge topToBottom = _Edge(gradients, minVert, maxVert, 0);
    Edge topToMiddle = _Edge(gradients, minVert, midVert, 0);
    Edge middleToBottom = _Edge(gradients, midVert, maxVert, 1);
    
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
        ScanLine(buffer, &gradients, left, right, y);
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
        ScanLine(buffer, &gradients, left, right, y);
        StepEdge(left);
        StepEdge(right);
    }
}

void FillTriangle(BackBuffer *buffer, Vertex v0, Vertex v1, Vertex v2)
{
    Vertex minVert = v0; 
    Vertex midVert = v1;
    Vertex maxVert = v2;
    minVert.pos = ToScreenSpace(buffer, v0.pos);
    midVert.pos = ToScreenSpace(buffer, v1.pos);
    maxVert.pos = ToScreenSpace(buffer, v2.pos);

    // TODO: Create a vertex struct to not have to swap 
    // interpolants values as well

    if(midVert.pos.y < minVert.pos.y)
    {
        Vertex temp = midVert;
        midVert = minVert;
        minVert = temp;
    }
    if(maxVert.pos.y < midVert.pos.y)
    {
        Vertex temp = midVert;
        midVert = maxVert;
        maxVert = temp;
    }
    if(midVert.pos.y < minVert.pos.y)
    {
        Vertex temp = midVert;
        midVert = minVert;
        minVert = temp;
    }
    
    V2F32 screenMin = _V2F32(minVert.pos.x, minVert.pos.y);
    V2F32 screenMid = _V2F32(midVert.pos.x, midVert.pos.y);
    V2F32 screenMax = _V2F32(maxVert.pos.x, maxVert.pos.y);

    F32 areaTimeTwo = CrossV2F32(SubV2F32(screenMax, screenMin), 
                                 SubV2F32(screenMid, screenMin));
    B8 handness = (areaTimeTwo >= 0) ? 1 : 0;
    ScanTriangle(buffer, screenMin, screenMid, screenMax, handness, 
                 minVert.color, midVert.color, maxVert.color);
}

V4F32 ToScreenSpace(BackBuffer *buffer, V4F32 v)
{
    I32 halfWidth = 0.5f*buffer->width;
    I32 halfHeight = 0.5f*buffer->height;
    V4F32 result = _V4F32(v.x, v.y, v.z, v.w);
    if(v.w != 0)
    {
        result.x /= v.w;
        result.y /= v.w;
        result.z /= v.w;
        result.w /= v.w;
    }
    result.x = (result.x*halfWidth) + halfWidth;
    result.y = (result.y*halfHeight) + halfHeight;

    return result;
}
