#ifndef TLIB_TYPES_H
#define TLIB_TYPES_H

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;
typedef float F32;
typedef uint8_t B8;
typedef uint32_t B32;

// TODO: Puts this defines in a base file

#include <assert.h>

#define Assert(value) assert(value)

#define Align64(value) (((value) + 0x7) & ~0x7)
#define Align32(value) (((value) + 0x3) & ~0x3)

#define ArrayCount(array) (sizeof(array)/sizeof(array[0]))

#endif //TLIB_TYPES_H
