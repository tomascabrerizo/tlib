#ifndef TLIB_WIN32_H
#define TLIB_WIN32_H

#include <Windows.h>

struct Win32BackBuffer
{
    HDC bufferDC;
    HBITMAP bitmap;
    BITMAPINFO bitmapInfo;
    U32 *pixels;
    U32 width, height;
};

#define NUMBER_OF_STARS 512

struct StarField
{
    V3F32 pos[NUMBER_OF_STARS];
    V3F32 color[NUMBER_OF_STARS];
};

#endif //TLIB_WIN32_H
