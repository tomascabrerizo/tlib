#ifndef TLIB_WIN32_H
#define TLIB_WIN32_H

#include <Windows.h>

typedef struct
{
    HDC bufferDC;
    HBITMAP bitmap;
    BITMAPINFO bitmapInfo;
    U32 *pixels;
    U32 width, height;
} Win32BackBuffer;

#define NUMBER_OF_STARS 512

typedef struct
{
    V3F32 pos[NUMBER_OF_STARS];
    V3F32 color[NUMBER_OF_STARS];
} StarField;

#endif //TLIB_WIN32_H
