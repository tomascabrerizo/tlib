#ifndef TLIB_MESH_H
#define TLIB_MESH_H

#include "tlib_types.h"
#include "tlib_platform.h"
#include "tlib_string.h"
#include "tlib_math.h"

struct ObjResult
{
    V3F32 *vertex;
    U32 vertexCount;

    V2F32 *texCoord;
    U32 texCoordCount;

    U32 *indices;
    U32 indicesCout;
};

inline ObjResult LoadObjFile(Arena *arena, char *fileName)
{
    ObjResult result = {};

    Arena fileArena = CreateArena(&PlatformCreateMemory());
    FileRes objFile = PlatformReadFile(&fileArena, fileName);

    String objContent = _String((char *)objFile.data);
    
    // NOTE: First pass get the buffers sizes
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
                    case ' ': { ++result.vertexCount; } break;
                    case 't': { ++result.texCoordCount; } break;
                    case 'n': { } break;
                }
            }break;
            case 'f':
            {
                ++result.indicesCout;
            }break;
        }
    }

    // NOTE: Alloc all the necessary buffers in the arena
    result.vertex = (V3F32 *)PushArena(arena, result.vertexCount*sizeof(V3F32));
    result.texCoord = (V2F32 *)PushArena(arena, result.texCoordCount*sizeof(V2F32));
    result.indices = (U32 *)PushArena(arena, result.indicesCout*sizeof(U32)*3);
    
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
                        V3F32 *vertex = result.vertex + vertexCount;
                        vertex->x = StringToFloat(StringSplit(&line, ' '));
                        vertex->y = StringToFloat(StringSplit(&line, ' '));
                        vertex->z = StringToFloat(StringSplit(&line, ' '));
                        ++vertexCount;
                        printf("v:(%f, %f, %f)\n", vertex->x, vertex->y, vertex->z);

                    } break;
                    case 't': 
                    {
                        line.data += 3;
                        V2F32 *texCoord = result.texCoord + texCoordCount;
                        texCoord->x = StringToFloat(StringSplit(&line, ' '));
                        texCoord->y = StringToFloat(StringSplit(&line, ' '));
                        ++texCoordCount;
                        printf("vt:(%f, %f)\n", texCoord->x, texCoord->y);
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
                
                result.indices[indicesCount + 0] = StringToInt(StringSplit(&vertIndex0, '/')); 
                result.indices[indicesCount + 1] = StringToInt(StringSplit(&vertIndex1, '/')); 
                result.indices[indicesCount + 2] = StringToInt(StringSplit(&vertIndex2, '/')); 
                
                printf("idc:(%d, %d, %d)\n", result.indices[indicesCount + 0],
                                             result.indices[indicesCount + 1],
                                             result.indices[indicesCount + 2]);

                indicesCount += 3;

            }break;
        }
    }

    // NOTE: Release the file arena, we dont need it any more
    ReleaseArena(&fileArena);
    
    return result;
}

#endif // TLIB_MESH_H

