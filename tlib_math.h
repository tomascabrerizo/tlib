#ifndef TLIB_MATH_H
#define TLIB_MATH_H

#include "tlib_types.h"

// NOTE: Random numbers
F32 RandomF32(void);

// NOTE: Trigonometry functions
#define NUM_PI 3.14159265359f

F32 ToRadians(F32 angle);

F32 CosF32(F32 angle);
F32 SinF32(F32 angle);
F32 TanF32(F32 angle);

// NOTE: Common math funtions
I32 CeilF32I32(F32 value);
F32 Clamp01(F32 num);
F32 SquareRootF32(F32 value);
F32 Lerp(F32 a, F32 b, F32 t);
F32 AbsF32(F32 num);

// NOTE: Vectors structs
typedef struct
{
    F32 x;
    F32 y;
} V2F32;

typedef struct
{
    F32 x;
    F32 y;
    F32 z;
} V3F32;

typedef struct 
{
    F32 x;
    F32 y;
    F32 z;
    F32 w;
} V4F32;

// NOTE: _Funtions are constructors for structs
V2F32 _V2F32(F32 x, F32 y);
V3F32 _V3F32(F32 x, F32 y, F32 z);
V4F32 _V4F32(F32 x, F32 y, F32 z, F32 w);

// NOTE: Simples getters
F32 GetAxisV4F32(V4F32 v, U32 axis);

// NOTE: Vector Adds
V2F32 AddV2F32(V2F32 a, V2F32 b);
V3F32 AddV3F32(V3F32 a, V3F32 b);
V4F32 AddV4F32(V4F32 a, V4F32 b);

// NOTE: Vector Subtract 
V2F32 SubV2F32(V2F32 a, V2F32 b);
V3F32 SubV3F32(V3F32 a, V3F32 b);
V4F32 SubV4F32(V4F32 a, V4F32 b);

// NOTE: Vector scale by value 
V2F32 ScaleV2F32(V2F32 a, F32 value);
V3F32 ScaleV3F32(V3F32 a, F32 value);
V4F32 ScaleV4F32(V4F32 a, F32 value);

// NOTE: Vector divide by value 
V2F32 DivV2F32(V2F32 a, F32 value);
V3F32 DivV3F32(V3F32 a, F32 value);
V4F32 DivV4F32(V4F32 a, F32 value);

// NOTE: Vector Dot product
F32 DotV2F32(V2F32 a, V2F32 b);
F32 DotV3F32(V3F32 a, V3F32 b);
F32 DotV4F32(V4F32 a, V4F32 b);

// NOTE: Vector Cross product 
F32 CrossV2F32(V2F32 a, V2F32 b);
V3F32 CrossV3F32(V3F32 a, V3F32 b);

// NOTE: Vector Lenght 
F32 LenghtSqrtV2F32(V2F32 a);
F32 LenghtV2F32(V2F32 a);

F32 LenghtSqrtV3F32(V3F32 a);
F32 LenghtV3F32(V3F32 a);

F32 LenghtSqrtV4F32(V4F32 a);
F32 LenghtV4F32(V4F32 a);

// NOTE: Vector Lerp functions
V2F32 LerpV2F32(V2F32 a, V2F32 b, F32 t);
V3F32 LerpV3F32(V3F32 a, V3F32 b, F32 t);
V4F32 LerpV4F32(V4F32 a, V4F32 b, F32 t);

// NOTE: Matrix structs
typedef struct
{
    F32 m[4][4];
} M4F32;

// NOTE: Matrix functions 
M4F32 IdentityM4F32(void);
M4F32 ScaleM4F32(F32 scale);
M4F32 TranslateM4F32(V3F32 translate);

M4F32 RotateXM4F32(F32 angle);
M4F32 RotateYM4F32(F32 angle);
M4F32 RotateZM4F32(F32 angle);

M4F32 MultM4F32(M4F32 a, M4F32 b);
V4F32 MultM4F32V4F32(M4F32 matrix, V4F32 v);

M4F32 PerspectiveM4F32(F32 fov, F32 aspect, F32 near, F32 far);

#endif // TLIB_MATH_H
