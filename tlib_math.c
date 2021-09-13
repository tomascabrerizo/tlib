#include "tlib_math.h"

#include <stdlib.h>
#include <math.h>

// NOTE: Random numbers
F32 RandomF32()
{
    F32 result = (F32)rand()/RAND_MAX;
    return result;
}

// NOTE: Trigonometry functions

#define NUM_PI 3.14159265359

F32 ToRadians(F32 angle)
{
    F32 result = (angle * NUM_PI) / 180.0f;
    return result;
}

F32 CosF32(F32 angle)
{
    F32 result = cosf(angle);
    return result;
}

F32 SinF32(F32 angle)
{
    F32 result = sinf(angle);
    return result;
}

F32 TanF32(F32 angle)
{
    F32 result = tanf(angle);
    return result;
}

// NOTE: Common math funtions

I32 CeilF32I32(F32 value)
{
    I32 result = ceilf(value);
    return result;
}

F32 Clamp01(F32 num)
{
    if(num > 1.0f)
    {
        num = 1.0f;
    }
    if(num < 0.0f)
    {
        num = 0.0f;
    }
    return num;

}

F32 SquareRootF32(F32 value)
{
    F32 result = sqrtf(value);
    return result;
}

F32 Lerp(F32 a, F32 b, F32 t)
{
    F32 result = (a + (b - a)*t);
    return result;
}

// NOTE: _Funtions are constructors for structs

V2F32 _V2F32(F32 x, F32 y)
{
    V2F32 result = {x, y};
    return result;
}

V3F32 _V3F32(F32 x, F32 y, F32 z)
{
    V3F32 result = {x, y, z};
    return result;
}

V4F32 _V4F32(F32 x, F32 y, F32 z, F32 w)
{
    V4F32 result = {x, y, z, w};
    return result;
}

// NOTE: Math functions 
// TODO: Maybe is a good idea to use operator overloading
// NOTE: Vector Adds
V2F32 AddV2F32(V2F32 a, V2F32 b)
{
    V2F32 result = {a.x + b.x, a.y + b.y};
    return result;
}

V3F32 AddV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

V4F32 AddV4F32(V4F32 a, V4F32 b)
{
    V4F32 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return result;
}

// NOTE: Vector Subtract 
V2F32 SubV2F32(V2F32 a, V2F32 b)
{
    V2F32 result = {a.x - b.x, a.y - b.y};
    return result;
}

V3F32 SubV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

V4F32 SubV4F32(V4F32 a, V4F32 b)
{
    V4F32 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return result;
}

// NOTE: Vector scale by value 
V2F32 ScaleV2F32(V2F32 a, F32 value)
{
    V2F32 result = {a.x * value, a.y * value};
    return result;
}

V3F32 ScaleV3F32(V3F32 a, F32 value)
{
    V3F32 result = {a.x * value, a.y * value, a.z * value};
    return result;
}

V4F32 ScaleV4F32(V4F32 a, F32 value)
{
    V4F32 result = {a.x * value, a.y * value, a.z * value, a.w * value};
    return result;
}

// NOTE: Vector divide by value 
V2F32 DivV2F32(V2F32 a, F32 value)
{
    V2F32 result = {a.x / value, a.y / value};
    return result;
}

V3F32 DivV3F32(V3F32 a, F32 value)
{
    V3F32 result = {a.x / value, a.y / value, a.z / value};
    return result;
}

V4F32 DivV4F32(V4F32 a, F32 value)
{
    V4F32 result = {a.x / value, a.y / value, a.z / value, a.w / value};
    return result;
}

// NOTE: Vector Dot product

F32 DotV2F32(V2F32 a, V2F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y);
    return result;
}

F32 DotV3F32(V3F32 a, V3F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y + a.z * b.z);
    return result;
}

F32 DotV4F32(V4F32 a, V4F32 b)
{
    F32 result = (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
    return result;
}

// NOTE: Vector Cross product 
F32 CrossV2F32(V2F32 a, V2F32 b)
{
    F32 result = {(a.x * b.y) - (b.x * a.y)};
    return result;
}

V3F32 CrossV3F32(V3F32 a, V3F32 b)
{
    V3F32 result = {
        (a.y*b.z - a.z*b.y),
        (a.z*b.x - a.x*b.z),
        (a.x*b.y - a.y*b.x)
    };
    return result;
}

// NOTE: Vector Lenght 

F32 LenghtSqrtV2F32(V2F32 a)
{
    F32 result = DotV2F32(a, a);
    return result;
}

F32 LenghtV2F32(V2F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV2F32(a));
    return result;
}

F32 LenghtSqrtV3F32(V3F32 a)
{
    F32 result = DotV3F32(a, a);
    return result;
}

F32 LenghtV3F32(V3F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV3F32(a));
    return result;
}

F32 LenghtSqrtV4F32(V4F32 a)
{
    F32 result = DotV4F32(a, a);
    return result;
}

F32 LenghtV4F32(V4F32 a)
{
    F32 result = SquareRootF32(LenghtSqrtV4F32(a));
    return result;
}

// NOTE: Vector Lerp functions

V2F32 LerpV2F32(V2F32 a, V2F32 b, F32 t)
{
    V2F32 result = AddV2F32(a, ScaleV2F32(SubV2F32(b, a), t));
    return result;
}

V3F32 LerpV3F32(V3F32 a, V3F32 b, F32 t)
{
    V3F32 result = AddV3F32(a, ScaleV3F32(SubV3F32(b, a), t));
    return result;
}

V4F32 LerpV4F32(V4F32 a, V4F32 b, F32 t)
{
    V4F32 result = AddV4F32(a, ScaleV4F32(SubV4F32(b, a), t));
    return result;
}

// NOTE: Matrix functions 

M4F32 MultM4F32(M4F32 a, M4F32 b)
{
    M4F32 result = {0};
    for(I32 j = 0; j < 4; ++j)
    {
        for(I32 i = 0; i < 4; ++i)
        {
            result.m[j][i] = a.m[j][0] * b.m[0][i] +
                             a.m[j][1] * b.m[1][i] +
                             a.m[j][2] * b.m[2][i] +
                             a.m[j][3] * b.m[3][i];
        }
    }
    return result;
}

M4F32 IdentityM4F32()
{
    M4F32 result = {0};
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

M4F32 ScaleM4F32(F32 scale)
{
    M4F32 result = IdentityM4F32();
    result.m[0][0] = scale;
    result.m[1][1] = scale;
    result.m[2][2] = scale;
    return result;
}

M4F32 TranslateM4F32(V3F32 translate)
{
    M4F32 result = IdentityM4F32();
    result.m[0][3] = translate.x;
    result.m[1][3] = translate.y;
    result.m[2][3] = translate.z;
    return result;
}

M4F32 RotateXM4F32(F32 angle)
{
    M4F32 result = IdentityM4F32();
    F32 radAngle = ToRadians(angle);
    F32 sinOfA = SinF32(radAngle);
    F32 cosOfA = CosF32(radAngle);
    result.m[1][1] =  cosOfA;
    result.m[1][2] = -sinOfA;
    result.m[2][1] =  sinOfA;
    result.m[2][2] =  cosOfA;
    return result;
}

M4F32 RotateYM4F32(F32 angle)
{
    M4F32 result = IdentityM4F32();
    F32 radAngle = ToRadians(angle);
    F32 sinOfA = SinF32(radAngle);
    F32 cosOfA = CosF32(radAngle);
    result.m[0][0] =  cosOfA;
    result.m[0][2] =  sinOfA;
    result.m[2][0] = -sinOfA;
    result.m[2][2] =  cosOfA;
    return result;
}

M4F32 RotateZM4F32(F32 angle)
{
    M4F32 result = IdentityM4F32();
    F32 radAngle = ToRadians(angle);
    F32 sinOfA = SinF32(radAngle);
    F32 cosOfA = CosF32(radAngle);
    result.m[0][0] =  cosOfA;
    result.m[0][1] = -sinOfA;
    result.m[1][0] =  sinOfA;
    result.m[1][1] =  cosOfA;
    return result;
}

M4F32 PerspectiveM4F32(F32 fov, F32 aspect, F32 near, F32 far)
{
    F32 tanHalfFov = TanF32(ToRadians(fov*0.5f));
    F32 range = (far - near);
    M4F32 result = {0};
    result.m[0][0] = 1.0f / (aspect*tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = far / range;
    result.m[2][3] = (-far*near) / range;
    result.m[3][2] = 1.0f;
    return result;
}

V4F32 MultM4F32V4F32(M4F32 matrix, V4F32 v)
{
    V4F32 result = {0};
    result.x = ((matrix.m[0][0] * v.x) + (matrix.m[0][1] * v.y) + 
                (matrix.m[0][2] * v.z) + (matrix.m[0][3] * v.w));
    
    result.y = ((matrix.m[1][0] * v.x) + (matrix.m[1][1] * v.y) + 
                (matrix.m[1][2] * v.z) + (matrix.m[1][3] * v.w));
    
    result.z = ((matrix.m[2][0] * v.x) + (matrix.m[2][1] * v.y) + 
                (matrix.m[2][2] * v.z) + (matrix.m[2][3] * v.w));
    
    result.w = ((matrix.m[3][0] * v.x) + (matrix.m[3][1] * v.y) + 
                (matrix.m[3][2] * v.z) + (matrix.m[3][3] * v.w));
    return result;
}

