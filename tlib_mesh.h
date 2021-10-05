#ifndef TLIB_MESH_H
#define TLIB_MESH_H

#include "tlib_render.h"
#include "tlib_types.h"
#include "tlib_platform.h"
#include "tlib_string.h"
#include "tlib_math.h"

typedef struct
{
    Vertex *vertex;
    U32 vertexCount;
} Mesh;

typedef struct
{
    V3F32 *vertex;
    V2F32 *texCoord;
    U32 *indices;
    U32 *texIndices;
    
    U32 vertexCount;
    U32 texCoordCount;
    U32 indicesCout;
} ObjResult;

Mesh LoadObjFile(Arena *arena, char *fileName);

#endif // TLIB_MESH_H

