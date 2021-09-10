#ifndef TLIB_PLATFORM_H
#define TLIB_PLATFORM_H

#include "tlib_types.h"
#include "tlib_render.h"

// NOTE: Functions that the codebase need to use and are implemented in 
// the diferents platform layers

#define KB(value) (value*1024LL)
#define MB(value) (KB(value)*1024LL)
#define GB(value) (MB(value)*1024LL)

typedef void *MemoryReserve(size_t size);
typedef void MemoryChage(void *ptr, size_t size);

// NOTE: Memory virtual table
struct Memory
{
    MemoryReserve *Reserve;
    MemoryChage *Commit;
    MemoryChage *Decommit;
    MemoryChage *Release;
};

Memory PlatformCreateMemory();

struct Arena
{
    Memory *platform;
    U8 *memory;
    size_t pos;
    size_t size;
    size_t capacity;
};

#define DEFAULT_RESERVE_SIZE GB(1);
#define DEFAULT_COMMIT_SIZE MB(64);

Arena CreateArena(Memory* platformMemory);
void *PushArena(Arena *arena, size_t size);
void ReleaseArena(Arena *arena);

struct FileRes
{
    void *data;
    size_t size;
};

FileRes PlatformReadFile(Arena *arena, char *fileName);

// NOTE: Functions that platform need to use 
void GameInit(BackBuffer *backBuffer);
void GameUpdateAndRender(BackBuffer *backBuffer, F32 dt);

#endif // TLIB_PLATFORM_H
