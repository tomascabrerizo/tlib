#include "tlib_render.h"

// NOTE: Gradients functions

Gradients _Gradients(V2F32 v0, V2F32 v1, V2F32 v2, 
                     V4F32 c0, V4F32 c1, V4F32 c2,
                     V2F32 t0, V2F32 t1, V2F32 t2)
{
    Gradients result = {};
    
    F32 oneOverDX = 1.0f / ((v1.x - v2.x)*(v0.y - v2.y) - (v0.x - v2.x)*(v1.y - v2.y));
    F32 oneOverDY = -oneOverDX;
    
    // NOTE: Init gradient color
    result.color[0] = c0;
    result.color[1] = c1;
    result.color[2] = c2;

    V4F32 dColorX = SubV4F32(ScaleV4F32(SubV4F32(c1, c2), (v0.y - v2.y)), 
                             ScaleV4F32(SubV4F32(c0, c2), (v1.y - v2.y)));

    V4F32 dColorY = SubV4F32(ScaleV4F32(SubV4F32(c1, c2), (v0.x - v2.x)), 
                             ScaleV4F32(SubV4F32(c0, c2), (v1.x - v2.x)));

    result.colorXStep = ScaleV4F32(dColorX, oneOverDX);
    result.colorYStep = ScaleV4F32(dColorY, oneOverDY);
    
    // NOTE: Init gradient texCoord 
    result.texCoord[0] = t0;
    result.texCoord[1] = t1;
    result.texCoord[2] = t2;

    V2F32 dtexCoordX = SubV2F32(ScaleV2F32(SubV2F32(t1, t2), (v0.y - v2.y)), 
                                ScaleV2F32(SubV2F32(t0, t2), (v1.y - v2.y)));

    V2F32 dtexCoordY = SubV2F32(ScaleV2F32(SubV2F32(t1, t2), (v0.x - v2.x)), 
                                ScaleV2F32(SubV2F32(t0, t2), (v1.x - v2.x)));

    result.texCoordXStep = ScaleV2F32(dtexCoordX, oneOverDX);
    result.texCoordYStep = ScaleV2F32(dtexCoordY, oneOverDY);

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
    
    // NOTE: Init edge color
    result.color = AddV4F32(AddV4F32(gradients.color[minIndexY], ScaleV4F32(gradients.colorYStep, yPreStep)),
                            ScaleV4F32(gradients.colorXStep, xPreStep));
    result.colorStep = AddV4F32(gradients.colorYStep, ScaleV4F32(gradients.colorXStep, result.xStep));
    
    // NOTE: Init edge texCoord 
    result.texCoord = AddV2F32(AddV2F32(gradients.texCoord[minIndexY], ScaleV2F32(gradients.texCoordYStep, yPreStep)),
                               ScaleV2F32(gradients.texCoordXStep, xPreStep));
    result.texCoordStep = AddV2F32(gradients.texCoordYStep, ScaleV2F32(gradients.texCoordXStep, result.xStep));

    return result;
}

void StepEdge(Edge *edge)
{
    edge->x += edge->xStep;
    edge->color = AddV4F32(edge->color, edge->colorStep);
    edge->texCoord = AddV2F32(edge->texCoord, edge->texCoordStep);
}

// NOTE: Vertex functions

Vertex _Vertex(F32 x, F32 y, F32 z, F32 red, F32 green, F32 blue, F32 u, F32 v)
{
    Vertex result = { _V4F32(x, y, z, 1), _V4F32(red, blue, green, 1.0f), _V2F32(u, v)};
    return result;
}

// NOTE: Rasterizer functions

void DrawPixel(BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    buffer->pixels[y * buffer->width + x] = color;
}

void CopyPixelFromBitmap(BackBuffer *buffer, Bitmap *bitmap, U32 x, U32 y, U32 srcX, U32 srcY)
{
    
    buffer->pixels[y * buffer->width + x] = bitmap->pixels[srcY * bitmap->width + srcX];
}


void ScanLine(BackBuffer *buffer, Bitmap *bitmap, Gradients *gradients, Edge *left, Edge *right, I32 y)
{
    I32 startX = CeilF32I32(left->x);
    I32 endX = CeilF32I32(right->x);

    F32 xPreStep = (startX - left->x);

    V4F32 minColor = AddV4F32(left->color, ScaleV4F32(gradients->colorXStep, xPreStep));
    V4F32 maxColor = AddV4F32(right->color, ScaleV4F32(gradients->colorXStep, xPreStep));
    
    // TODO: Need to take care of xPreStep but this code have a bug
    //V2F32 minTexCoord = AddV2F32(left->texCoord, ScaleV2F32(gradients->texCoordXStep, xPreStep));
    //V2F32 maxTexCoord = AddV2F32(right->texCoord, ScaleV2F32(gradients->texCoordXStep, xPreStep));
    V2F32 minTexCoord = left->texCoord;
    V2F32 maxTexCoord = right->texCoord;
    
    // TODO: Should be able to step whit only xStep (no lerp needed)
    //F32 lerpT = 0;
    //F32 lerpStep = 1.0f / (F32)(endX - startX);

    for(I32 x = startX; x < endX; ++x)
    {
        //V4F32 color = ScaleV4F32(LerpV4F32(minColor, maxColor, lerpT), 255);
        V4F32 color = ScaleV4F32(minColor, 255);
        DrawPixel(buffer, x, y, color.x, color.y, color.z);
        minColor = AddV4F32(minColor, gradients->colorXStep);

        //V2F32 srcTexCoord = LerpV2F32(minTexCoord, maxTexCoord, lerpT);
        V2F32 srcTexCoord = minTexCoord;
        U32 srcX = (U32)(srcTexCoord.x * (bitmap->width  - 1) + 0.5f);
        U32 srcY = (U32)(srcTexCoord.y * (bitmap->height - 1) + 0.5f);
        
        CopyPixelFromBitmap(buffer, bitmap, x, y, srcX, srcY);
        minTexCoord = AddV2F32(minTexCoord, gradients->texCoordXStep);

        //lerpT += lerpStep;
    }
}

void ScanTriangle(BackBuffer *buffer, Bitmap *bitmap,
                  V2F32 minVert, V2F32 midVert, V2F32 maxVert, B8 handness,
                  V4F32 c0, V4F32 c1, V4F32 c2, V2F32 t0, V2F32 t1, V2F32 t2)
{
    Gradients gradients = _Gradients(minVert, midVert, maxVert, 
                                     c0, c1, c2, t0, t1, t2);

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
        ScanLine(buffer, bitmap, &gradients, left, right, y);
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
        ScanLine(buffer, bitmap, &gradients, left, right, y);
        StepEdge(left);
        StepEdge(right);
    }
}

void FillTriangle(BackBuffer *buffer, Bitmap *bitmap, Vertex v0, Vertex v1, Vertex v2)
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
    ScanTriangle(buffer, bitmap, screenMin, screenMid, screenMax, handness, 
                 minVert.color, midVert.color, maxVert.color,
                 minVert.texCoord, midVert.texCoord, maxVert.texCoord);
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
