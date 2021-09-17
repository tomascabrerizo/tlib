#include "tlib_render.h"

// NOTE: Vertex functions

Vertex _Vertex(F32 x, F32 y, F32 z, F32 red, F32 green, F32 blue, F32 u, F32 v)
{
    Vertex result = { _V4F32(x, y, z, 1.0f), _V4F32(red, blue, green, 1.0f), _V2F32(u, v)};
    return result;
}

Vertex VertexLerp(Vertex v0, Vertex v1, F32 t)
{
   Vertex result = {0};
   result.pos = LerpV4F32(v0.pos, v1.pos, t); 
   result.color = LerpV4F32(v0.color, v1.color, t); 
   result.texCoord = LerpV2F32(v0.texCoord, v1.texCoord, t); 
   return result;
}

// NOTE: Gradients functions

Gradients _Gradients(Vertex v0, Vertex v1, Vertex v2)
{
    Gradients result = {0};
    
    F32 oneOverDX = 1.0f / ((v1.pos.x - v2.pos.x)*(v0.pos.y - v2.pos.y) -
                            (v0.pos.x - v2.pos.x)*(v1.pos.y - v2.pos.y));
    F32 oneOverDY = -oneOverDX;
    
    // NOTE: Init Z Depth
    result.zDepth[0] = v0.pos.z;
    result.zDepth[1] = v1.pos.z;
    result.zDepth[2] = v2.pos.z;
    
    F32 dZDepthX = ((result.zDepth[1] - result.zDepth[2])*(v0.pos.y - v2.pos.y) -
                    (result.zDepth[0] - result.zDepth[2])*(v1.pos.y - v2.pos.y));

    F32 dZDepthY = ((result.zDepth[1] - result.zDepth[2])*(v0.pos.x - v2.pos.x) -
                    (result.zDepth[0] - result.zDepth[2])*(v1.pos.x - v2.pos.x));

    result.zDepthXStep = dZDepthX * oneOverDX;
    result.zDepthYStep = dZDepthY * oneOverDY;

    // NOTE: Init One over Z
    result.oneOverZ[0] = 1.0f / v0.pos.w;
    result.oneOverZ[1] = 1.0f / v1.pos.w;
    result.oneOverZ[2] = 1.0f / v2.pos.w;
    
    F32 dOneOverZX = ((result.oneOverZ[1] - result.oneOverZ[2])*(v0.pos.y - v2.pos.y) -
                      (result.oneOverZ[0] - result.oneOverZ[2])*(v1.pos.y - v2.pos.y));

    F32 dOneOverZY = ((result.oneOverZ[1] - result.oneOverZ[2])*(v0.pos.x - v2.pos.x) -
                      (result.oneOverZ[0] - result.oneOverZ[2])*(v1.pos.x - v2.pos.x));
    
    result.oneOverZXStep = dOneOverZX * oneOverDX;
    result.oneOverZYStep = dOneOverZY * oneOverDY;

    // NOTE: Init gradient color
    result.color[0] = ScaleV4F32(v0.color, result.oneOverZ[0]);
    result.color[1] = ScaleV4F32(v1.color, result.oneOverZ[1]);
    result.color[2] = ScaleV4F32(v2.color, result.oneOverZ[2]);

    V4F32 dColorX = SubV4F32(ScaleV4F32(SubV4F32(result.color[1], result.color[2]), (v0.pos.y - v2.pos.y)), 
                             ScaleV4F32(SubV4F32(result.color[0], result.color[2]), (v1.pos.y - v2.pos.y)));

    V4F32 dColorY = SubV4F32(ScaleV4F32(SubV4F32(result.color[1], result.color[2]), (v0.pos.x - v2.pos.x)), 
                             ScaleV4F32(SubV4F32(result.color[0], result.color[2]), (v1.pos.x - v2.pos.x)));

    result.colorXStep = ScaleV4F32(dColorX, oneOverDX);
    result.colorYStep = ScaleV4F32(dColorY, oneOverDY);
   
    // NOTE: Init gradient texCoord 
    result.texCoord[0] = ScaleV2F32(v0.texCoord, result.oneOverZ[0]);
    result.texCoord[1] = ScaleV2F32(v1.texCoord, result.oneOverZ[1]);
    result.texCoord[2] = ScaleV2F32(v2.texCoord, result.oneOverZ[2]);

    V2F32 dtexCoordX = SubV2F32(ScaleV2F32(SubV2F32(result.texCoord[1], result.texCoord[2]), (v0.pos.y - v2.pos.y)), 
                                ScaleV2F32(SubV2F32(result.texCoord[0], result.texCoord[2]), (v1.pos.y - v2.pos.y)));

    V2F32 dtexCoordY = SubV2F32(ScaleV2F32(SubV2F32(result.texCoord[1], result.texCoord[2]), (v0.pos.x - v2.pos.x)), 
                                ScaleV2F32(SubV2F32(result.texCoord[0], result.texCoord[2]), (v1.pos.x - v2.pos.x)));
    
    result.texCoordXStep = ScaleV2F32(dtexCoordX, oneOverDX);
    result.texCoordYStep = ScaleV2F32(dtexCoordY, oneOverDY);

    return result;
}

// NOTE: Edge functions

Edge _Edge(Gradients *gradients, Vertex start, Vertex end, U32 minIndexY)
{
    Edge result = {0};
    result.startY = CeilF32I32(start.pos.y);
    result.endY = CeilF32I32(end.pos.y);
    
    F32 yDist = (end.pos.y - start.pos.y); 
    F32 xDist = (end.pos.x - start.pos.x); 
    
    F32 yPreStep = result.startY - start.pos.y;

    result.xStep = xDist / yDist;
    result.x = start.pos.x + yPreStep * result.xStep;
    
    F32 xPreStep = result.x - start.pos.x;
    
    // NOTE: Init edge color
    result.color = AddV4F32(AddV4F32(gradients->color[minIndexY], ScaleV4F32(gradients->colorYStep, yPreStep)),
                            ScaleV4F32(gradients->colorXStep, xPreStep));
    result.colorStep = AddV4F32(gradients->colorYStep, ScaleV4F32(gradients->colorXStep, result.xStep));
    
    // NOTE: Init edge texCoord 
    result.texCoord = AddV2F32(AddV2F32(gradients->texCoord[minIndexY], ScaleV2F32(gradients->texCoordYStep, yPreStep)),
                               ScaleV2F32(gradients->texCoordXStep, xPreStep));
    result.texCoordStep = AddV2F32(gradients->texCoordYStep, ScaleV2F32(gradients->texCoordXStep, result.xStep));
    
    // NOTE: Init oneOverZ edge
    result.oneOverZ = gradients->oneOverZ[minIndexY] + (gradients->oneOverZYStep * yPreStep) + 
                                                       (gradients->oneOverZXStep * xPreStep);
    result.oneOverZStep = gradients->oneOverZYStep + (gradients->oneOverZXStep * result.xStep);

    // NOTE: Init Z depth edge
    result.zDepth = gradients->zDepth[minIndexY] + (gradients->zDepthYStep * yPreStep) + 
                                                   (gradients->zDepthXStep * xPreStep);
    result.zDepthStep = gradients->zDepthYStep + (gradients->zDepthXStep * result.xStep);

    return result;
}

void StepEdge(Edge *edge)
{
    edge->x += edge->xStep;
    edge->color = AddV4F32(edge->color, edge->colorStep);
    edge->texCoord = AddV2F32(edge->texCoord, edge->texCoordStep);
    edge->oneOverZ += edge->oneOverZStep;
    edge->zDepth += edge->zDepthStep;
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
    
    // TODO: Maybe recalculate this coordinates insted of using gradients
#if 0 
    F32 xDist = right->x - left->x;
    F32 depthXStep = (right->zDepth - left->zDepth) / xDist;
    F32 depth = left->zDepth + (depthXStep * xPreStep);
#else
    V4F32 minColor = AddV4F32(left->color, ScaleV4F32(gradients->colorXStep, xPreStep));
    V2F32 minTexCoord = AddV2F32(left->texCoord, ScaleV2F32(gradients->texCoordXStep, xPreStep));
    F32 minOneOverZ = left->oneOverZ + (gradients->oneOverZXStep * xPreStep);
    F32 depth = left->zDepth + (gradients->zDepthXStep * xPreStep);
#endif
    
    for(I32 x = startX; x < endX; ++x)
    {
        U32 zIndex = (y*buffer->width+x);
        if(depth < buffer->zBuffer[zIndex])
        {
            buffer->zBuffer[zIndex] = depth;
            F32 z = 1.0f/minOneOverZ;

            V4F32 color = ScaleV4F32(ScaleV4F32(minColor, z), 255);
            DrawPixel(buffer, x, y, (U8)color.x, (U8)color.y, (U8)color.z);
            minColor = AddV4F32(minColor, gradients->colorXStep);

            V2F32 srcTexCoord = minTexCoord;
            U32 srcX = (U32)(Clamp01(srcTexCoord.x*z) * (bitmap->width  - 1) + 0.5f);
            U32 srcY = (U32)(Clamp01(srcTexCoord.y*z) * (bitmap->height - 1) + 0.5f);
            
            CopyPixelFromBitmap(buffer, bitmap, x, y, srcX, srcY);
        }

        minTexCoord = AddV2F32(minTexCoord, gradients->texCoordXStep);
        minOneOverZ = minOneOverZ + gradients->oneOverZXStep;
        depth = depth + gradients->zDepthXStep;
    }
}

void ScanTriangle(BackBuffer *buffer, Bitmap *bitmap,
                  Vertex minVert, Vertex midVert, Vertex maxVert, B8 handness)
{
    Gradients gradients = _Gradients(minVert, midVert, maxVert);

    Edge topToBottom = _Edge(&gradients, minVert, maxVert, 0);
    Edge topToMiddle = _Edge(&gradients, minVert, midVert, 0);
    Edge middleToBottom = _Edge(&gradients, midVert, maxVert, 1);
    
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

void CopyVertexList(VertexList *dest, VertexList *src)
{
    dest->count = src->count;
    for(U32 i = 0; i < dest->count; ++i)
    {
        dest->vertex[i] = src->vertex[i];
    }
}

void ClipTriangleAxis(VertexList *vertexList, U32 axis, VertexList *result)
{
    if(vertexList->count == 0)
    {
        return;
    }

    // NOTE: Clip against the first plain
    Vertex *previusVertex = vertexList->vertex + (vertexList->count-1);
    I8 previusInside = GetAxisV4F32(previusVertex->pos, axis) <= previusVertex->pos.w ? 1 : -1;
    Vertex *currentVertex = vertexList->vertex;
    while(currentVertex != vertexList->vertex + vertexList->count)
    {
        I8 currentIside = GetAxisV4F32(currentVertex->pos, axis) <= currentVertex->pos.w ? 1 : -1;
        if((previusInside * currentIside) < 0)
        {
            // NOTE: Need to clip against plan = w 
            F32 lerpAmt =  (previusVertex->pos.w - GetAxisV4F32(previusVertex->pos, axis)) /
                          ((previusVertex->pos.w - GetAxisV4F32(previusVertex->pos, axis)) -
                          (currentVertex->pos.w - GetAxisV4F32(currentVertex->pos, axis)));

            result->vertex[result->count++] = VertexLerp(*previusVertex, *currentVertex, lerpAmt);
        }

        if(currentIside > 0)
        {
            result->vertex[result->count++] = *currentVertex; 
        }

        // NOTE: Advance the current vertex
        previusInside = currentIside;
        previusVertex = currentVertex;
        currentVertex++;
    }

    CopyVertexList(vertexList, result);
    result->count = 0;

    if(vertexList->count == 0)
    {
        return;
    }
    // NOTE: Clip against opposite plain
    previusVertex = vertexList->vertex + (vertexList->count-1);
    previusInside = -GetAxisV4F32(previusVertex->pos, axis) <= previusVertex->pos.w ? 1 : -1;
    currentVertex = vertexList->vertex;
    while(currentVertex != vertexList->vertex + vertexList->count)
    {
        I8 currentIside = (-GetAxisV4F32(currentVertex->pos, axis)) <= currentVertex->pos.w ? 1 : -1;
        if((previusInside * currentIside) < 0)
        {
            // NOTE: Need to clip against plan = w 
            F32 lerpAmt =  (previusVertex->pos.w + GetAxisV4F32(previusVertex->pos, axis)) /
                          ((previusVertex->pos.w + GetAxisV4F32(previusVertex->pos, axis)) -
                          (currentVertex->pos.w + GetAxisV4F32(currentVertex->pos, axis)));

            result->vertex[result->count++] = VertexLerp(*previusVertex, *currentVertex, lerpAmt);
        }

        if(currentIside > 0)
        {
            result->vertex[result->count++] = *currentVertex; 
        }

        // NOTE: Advance the current vertex
        previusInside = currentIside;
        previusVertex = currentVertex;
        currentVertex++;
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
    
    // NOTE: Face culling
    // TODO: Probably could be merge with handness
    V3F32 realMin = _V3F32(minVert.pos.x, minVert.pos.y, minVert.pos.z);
    V3F32 realMid = _V3F32(midVert.pos.x, midVert.pos.y, midVert.pos.z);
    V3F32 realMax = _V3F32(maxVert.pos.x, maxVert.pos.y, maxVert.pos.z);
    V3F32 culling = CrossV3F32(SubV3F32(realMax, realMin),
                               SubV3F32(realMid, realMin));
    if(culling.z > 0) return;

    // TODO: Create a vertex struct to not have to swap 
    // interpolants values as well

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
    if(maxVert.pos.y < midVert.pos.y)
    {
        Vertex temp = midVert;
        midVert = maxVert;
        maxVert = temp;
    }
    
    V2F32 screenMin = _V2F32(minVert.pos.x, minVert.pos.y);
    V2F32 screenMid = _V2F32(midVert.pos.x, midVert.pos.y);
    V2F32 screenMax = _V2F32(maxVert.pos.x, maxVert.pos.y);

    F32 areaTimeTwo = CrossV2F32(SubV2F32(screenMax, screenMin), 
                                 SubV2F32(screenMid, screenMin));
    B8 handness = (areaTimeTwo >= 0) ? 1 : 0;
    ScanTriangle(buffer, bitmap, minVert, midVert, maxVert, handness);
}

void DrawTriangle(BackBuffer *buffer, Bitmap *bitmap, Vertex v0, Vertex v1, Vertex v2)
{
    VertexList vertexList = {0};
    VertexList tmpList = {0};
    
    tmpList.vertex[tmpList.count++] = v0;
    tmpList.vertex[tmpList.count++] = v1;
    tmpList.vertex[tmpList.count++] = v2;

    ClipTriangleAxis(&tmpList, 0, &vertexList);
    tmpList.count = 0;
    ClipTriangleAxis(&vertexList, 1, &tmpList);
    vertexList.count = 0;
    ClipTriangleAxis(&tmpList, 2, &vertexList);
    tmpList.count = 0;
    
    if(vertexList.count > 0)
    {
        for(U32 i = 1; i < vertexList.count-1; ++i)
        {
            FillTriangle(buffer, bitmap, vertexList.vertex[0], vertexList.vertex[i], vertexList.vertex[i+1]);
        }
    }
};

V4F32 ToScreenSpace(BackBuffer *buffer, V4F32 v)
{
    V4F32 result = _V4F32(v.x, v.y, v.z, v.w);
    
    if(v.w != 0)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    
    F32 halfWidth = (0.5f * (F32)buffer->width);
    F32 halfHeight = (0.5f * (F32)buffer->height);
    
    result.x = (result.x * halfWidth) + (halfWidth - 0.5f);
    result.y = (result.y * -halfHeight) + (halfHeight - 0.5f);

    return result;
}
