#include "tlib_mesh.h"

Mesh LoadObjFile(Arena *arena, char *fileName)
{
    ObjResult objResult = {0};
    FileRes objFile = PlatformReadFile(fileName);
    String objContent = _String((char *)objFile.data);

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
                        V3F32 *vertex = DArrayPush(&objResult.vertex, V3F32);
                        vertex->x = StringToFloat(StringSplit(&line, ' '));
                        vertex->y = StringToFloat(StringSplit(&line, ' '));
                        vertex->z = StringToFloat(StringSplit(&line, ' '));
                        ++vertexCount;

                    } break;
                    case 't': 
                    {
                        line.data += 3;
                        V2F32 *texCoord = DArrayPush(&objResult.texCoord, V2F32);
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
                
                U32 *indices = DArrayPush(&objResult.indices, U32);
                U32 *texIndices = DArrayPush(&objResult.texIndices, U32);
                *indices = StringToInt(StringSplit(&vertIndex0, '/')); 
                *texIndices = StringToInt(StringSplit(&vertIndex0, '/')); 

                indices = DArrayPush(&objResult.indices, U32);
                texIndices = DArrayPush(&objResult.texIndices, U32);
                *indices = StringToInt(StringSplit(&vertIndex1, '/')); 
                *texIndices = StringToInt(StringSplit(&vertIndex1, '/')); 

                indices = DArrayPush(&objResult.indices, U32);
                texIndices = DArrayPush(&objResult.texIndices, U32);
                *indices = StringToInt(StringSplit(&vertIndex2, '/')); 
                *texIndices = StringToInt(StringSplit(&vertIndex2, '/')); 
                
                indicesCount += 3;

            }break;
        }
    }

    // NOTE: Free the obj file
    PlatformFreeFile(&objFile);
    
    Mesh result = {0};
    result.vertexCount = indicesCount;
    result.vertex = (Vertex *)PushArena(arena, result.vertexCount*sizeof(Vertex));
    for(U32 i = 0; i < result.vertexCount; ++i)
    {
        V3F32 vertex = objResult.vertex[objResult.indices[i]-1];
        result.vertex[i].pos = _V4F32(vertex.x, vertex.y, vertex.z, 1.0f); 
        result.vertex[i].texCoord = objResult.texCoord[objResult.texIndices[i]-1];
        result.vertex[i].color = _V4F32(1.0f, 0.0f, 0.0f, 1.0f); 
    }

    // NOTE: Delete all temporal arrays 
    DArrayRelease(&objResult.vertex);
    DArrayRelease(&objResult.texCoord);
    DArrayRelease(&objResult.indices);
    DArrayRelease(&objResult.texIndices);

    return result;
}
