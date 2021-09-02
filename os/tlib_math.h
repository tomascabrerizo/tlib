#ifndef TLIB_MATH_H
#define TLIB_MATH_H

#include "tlib_types.h"

#include <stdlib.h>
#include <math.h>

// NOTE: Random numbers
inline F32 RandomF32()
{
    F32 result = (F32)rand()/RAND_MAX;
    return result;
}

// NOTE: Common math funtions

inline I32 CeilF32I32(F32 value)
{
    I32 result = ceilf(value);
    return result;
}

inline F32 SquareRootF32(F32 value)
{
    F32 result = sqrtf(value);
    return result;
}

// NOTE: Math structs

struct V2F32
{
    F32 x;
    F32 y;
};

struct V3F32
{
    F32 x;
    F32 y;
    F32 z;
};

struct V4F32
{
    F32 x;
    F32 y;
    F32 z;
    F32 w;
};

// NOTE: _Funtions are constructors for structs

inline V2F32 _V2F32(F32 x, F32 y)
{
    V2F32 result = {x, y};
    return result;
}

inline V3F32 _V3F32(F32 x, F32 y, F32 z)
{
    V3F32 result = {x, y, z};
    return result;
}

inline V4F32 _V4F32(F32 x, F32 y, F32 z, F32 w)
{
    V4F32 result = {x, y, z, w};
    return result;
}

// NOTE: Math functions 
// TODO: Maybe is a good idea to use operator overloading
// NOTE: Vector Adds
inline V2F32 AddV2F32(V2F32 a, V2F32 b)
{
    V2F32 result = {a.x + b.x, a.y + b.y};
    return result;
}

inline V3F32 AddV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

inline V4F32 AddV4F32(V4F32 a, V4F32 b)
{
    V4F32 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return result;
}

// NOTE: Vector Subtract 
inline V2F32 SubV2F32(V2F32 a, V2F32 b)
{
    V2F32 result = {a.x - b.x, a.y - b.y};
    return result;
}

inline V3F32 SubV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

inline V4F32 SubV4F32(V4F32 a, V4F32 b)
{
    V4F32 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return result;
}

// NOTE: Vector scale by value 
inline V2F32 ScaleV2F32(V2F32 a, F32 value)
{
    V2F32 result = {a.x * value, a.y * value};
    return result;
}

inline V3F32 ScaleV3F32(V3F32 a, F32 value)
{
    V3F32 result = {a.x * value, a.y * value, a.z * value};
    return result;
}

inline V4F32 ScaleV4F32(V4F32 a, F32 value)
{
    V4F32 result = {a.x * value, a.y * value, a.z * value, a.w * value};
    return result;
}

// NOTE: Vector divide by value 
inline V2F32 DivV2F32(V2F32 a, F32 value)
{
    V2F32 result = {a.x / value, a.y / value};
    return result;
}

inline V3F32 DivV3F32(V3F32 a, F32 value)
{
    V3F32 result = {a.x / value, a.y / value, a.z / value};
    return result;
}

inline V4F32 DivV4F32(V4F32 a, F32 value)
{
    V4F32 result = {a.x / value, a.y / value, a.z / value, a.w / value};
    return result;
}

// NOTE: Vector Dot product

inline F32 DotV2F32(V2F32 a, V2F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y);
    return result;
}

inline F32 DotV3F32(V3F32 a, V3F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y + a.z * b.z);
    return result;
}

inline F32 DotV4F32(V4F32 a, V4F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
    return result;
}

// NOTE: Vector Cross product 
inline F32 CrossV2F32(V2F32 a, V2F32 b)
{
    F32 result = {(a.x * b.y) - (b.x * a.y)};
    return result;
}

inline V3F32 CrossV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {
        (a.y*b.z - a.z*b.y),
        (a.z*b.x - a.x*b.z),
        (a.x*b.y - a.y*b.x)
    };
    return result;
}

// NOTE: Vector Lenght 

inline F32 LenghtSqrtV2F32(V2F32 a)
{
    F32 result = DotV2F32(a, a);
    return result;
}

inline F32 LenghtV2F32(V2F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV2F32(a));
    return result;
}

inline F32 LenghtSqrtV3F32(V3F32 a)
{
    F32 result = DotV3F32(a, a);
    return result;
}

inline F32 LenghtV2F32(V3F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV3F32(a));
    return result;
}

inline F32 LenghtSqrtV4F32(V4F32 a)
{
    F32 result = DotV4F32(a, a);
    return result;
}

inline F32 LenghtV4F32(V4F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV4F32(a));
    return result;
}


#endif // TLIB_MATH_H
