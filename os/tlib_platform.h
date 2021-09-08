#ifndef TLIB_PLATFORM_H
#define TLIB_PLATFORM_H

#include "tlib_types.h"
#include "tlib_render.h"

// NOTE: Functions that the codebase need to use and are implemented in 
// the diferents platform layers

#define Kilobytes(value) (value*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)

struct Arena
{
    U8 *data;
    size_t usage;
    size_t size;
};

#define ARENA_DEAULF_RESERVE_SIZE Gigabytes(1);
#define ARENA_DEAULF_COMMIT_SIZE Megabytes(64);

Arena PlatformArenaCreate();

void PlatformArenaCommit(Arena *arena, size_t size);
void PlatformArenaDecommit(Arena *arena);

void PlatformArenaReserve(Arena *arena, size_t size);
void PlatformArenaFree(Arena *arena);

void *PlatformReadFile(char *fileName);


// NOTE: Functions that platform need to use 
void GameInit(BackBuffer *backBuffer);
void GameUpdateAndRender(BackBuffer *backBuffer, F32 dt);


#endif // TLIB_PLATFORM_H
