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
    U32 vertexCount;

    V2F32 *texCoord;
    U32 texCoordCount;

    U32 *indices;
    U32 *texIndices;
    U32 indicesCout;
} ObjResult;

inline Mesh LoadObjFile(Arena *arena, char *fileName)
{
    ObjResult objResult = {0};
    
    // TODO: Allocate memory with PlatformCreateMemory.Reserve instead of create and arena
    Memory memory = PlatformCreateMemory();
    Arena fileArena = CreateArena(&memory);
    FileRes objFile = PlatformReadFile(&fileArena, fileName);

    String objContent = _String((char *)objFile.data);
    
    // NOTE: First pass get the buffers sizes
    // TODO: Maybe dont allocate space for the different buffer,
    // just add them as it goes becouse there are in order
    String countString = objContent;
    while(countString.size > 0)
    {
        String line = StringTrim(StringSplit(&countString, '\n'));
        switch(*line.data)
        {
            case 'v':
            {
                switch(*(line.data + 1))
                {
                    case ' ': { ++objResult.vertexCount; } break;
                    case 't': { ++objResult.texCoordCount; } break;
                    case 'n': { } break;
                }
            }break;
            case 'f':
            {
                ++objResult.indicesCout;
            }break;
        }
    }

    // NOTE: Alloc all the necessary buffers in the arena
    objResult.vertex = (V3F32 *)PushArena(&fileArena, objResult.vertexCount*sizeof(V3F32));
    objResult.texCoord = (V2F32 *)PushArena(&fileArena, objResult.texCoordCount*sizeof(V2F32));
    objResult.indices = (U32 *)PushArena(&fileArena, objResult.indicesCout*sizeof(U32)*3);
    objResult.texIndices = (U32 *)PushArena(&fileArena, objResult.indicesCout*sizeof(U32)*3);
    
    U32 vertexCount = 0;
    U32 texCoordCount = 0;
    U32 indicesCount = 0;
    String parseString = objContent;
    while(parseString.size > 0)
    {
        String line = StringTrim(StringSplit(&parseString, '\n'));
        switch(*line.data)
        {
            case 'v':
            {
                switch(*(line.data + 1))
                {
                    case ' ': 
                    {
                        line.data += 2;
                        V3F32 *vertex = objResult.vertex + vertexCount;
                        vertex->x = StringToFloat(StringSplit(&line, ' '));
                        vertex->y = StringToFloat(StringSplit(&line, ' '));
                        vertex->z = StringToFloat(StringSplit(&line, ' '));
                        ++vertexCount;

                    } break;
                    case 't': 
                    {
                        line.data += 3;
                        V2F32 *texCoord = objResult.texCoord + texCoordCount;
                        texCoord->x = StringToFloat(StringSplit(&line, ' '));
                        texCoord->y = StringToFloat(StringSplit(&line, ' '));
                        ++texCoordCount;
                    } break;
                    case 'n': 
                    {
                        // TODO: Implement normal parser
                    } break;
                }
            }break;
            case 'f':
            {
                line.data += 2;
                String vertIndex0 = StringSplit(&line, ' ');
                String vertIndex1 = StringSplit(&line, ' ');
                String vertIndex2 = StringSplit(&line, ' ');
                
                objResult.indices[indicesCount + 0] = StringToInt(StringSplit(&vertIndex0, '/')); 
                objResult.texIndices[indicesCount + 0] = StringToFloat(StringSplit(&vertIndex0, '/')); 

                objResult.indices[indicesCount + 1] = StringToInt(StringSplit(&vertIndex1, '/')); 
                objResult.texIndices[indicesCount + 1] = StringToFloat(StringSplit(&vertIndex1, '/')); 

                objResult.indices[indicesCount + 2] = StringToInt(StringSplit(&vertIndex2, '/')); 
                objResult.texIndices[indicesCount + 2] = StringToFloat(StringSplit(&vertIndex2, '/')); 
                
                indicesCount += 3;

            }break;
        }
    }
    
    // TODO: Create the final mesh in the destination arena
    Mesh result = {0};
    result.vertexCount = objResult.indicesCout*3;
    result.vertex = (Vertex *)PushArena(arena, result.vertexCount*sizeof(Vertex));
    for(U32 i = 0; i < result.vertexCount; ++i)
    {
        V3F32 vertex = objResult.vertex[objResult.indices[i]-1];
        result.vertex[i].pos = _V4F32(vertex.x, vertex.y, vertex.z, 1.0f); 
        // TODO: Get texCoord indices
        result.vertex[i].texCoord = objResult.texCoord[objResult.texIndices[i]-1];
        result.vertex[i].color = _V4F32(1.0f, 0.0f, 0.0f, 1.0f); 
    }

    // NOTE: Release the file arena, we dont need it any more
    ReleaseArena(&fileArena);
    
    return result;
}

#endif // TLIB_MESH_H

