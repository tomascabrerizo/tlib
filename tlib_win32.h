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
    F32 *zBuffer;
} Win32BackBuffer;

#endif //TLIB_WIN32_H
