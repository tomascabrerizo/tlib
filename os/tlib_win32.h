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

struct StarField
{
    V3F32 pos[512];
};

#endif //TLIB_WIN32_H
