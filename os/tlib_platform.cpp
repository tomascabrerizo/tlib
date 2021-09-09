#include "tlib_platform.h"

Arena CreateArena(Memory* platformMemory)
{
    Arena result = {};
   
    result.platform = platformMemory;

    result.capacity = DEFAULT_RESERVE_SIZE;
    result.memory = (U8 *)result.platform->Reserve(result.capacity);

    result.size = DEFAULT_COMMIT_SIZE;
    result.platform->Commit(result.memory, result.size);

    return result;
}

void *PushArena(Arena *arena, size_t size)
{
    void *result = 0;
    size_t alignSize = Align64(size);
    if(arena->pos + alignSize <= arena->size)
    {
        result = arena->memory + arena->pos;
        arena->pos += alignSize;
    }
    else
    {
        size_t commitSize = DEFAULT_COMMIT_SIZE;
        Assert(arena->size + commitSize <= arena->capacity);
        arena->platform->Commit(arena->memory, commitSize);
        arena->size += commitSize;
    }
    return result;
}
