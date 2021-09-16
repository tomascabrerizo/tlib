#include "tlib_main.c"
#include "tlib_render.c"
#include "tlib_platform.c"
#include "tlib_string.c"
#include "tlib_math.c"
#include "tlib_mesh.c"

#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_platform.h"
#include "tlib_darray.h"
#include "tlib_win32.h"

#define WINDOW_WIDTH 800 
#define WINDOW_HEIGHT 600

// NOTE: Platform code
void *Win32MemoryReserve(size_t size)
{   
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

void Win32MemoryCommit(void *ptr, size_t size)
{
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

void Win32MemoryDecommit(void *ptr, size_t size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void Win32MemoryRelease(void *ptr, size_t size)
{
    VirtualFree(ptr, size, MEM_RELEASE);
}

Memory PlatformCreateMemory(void)
{
    Memory result = {0};
    result.Reserve = Win32MemoryReserve;
    result.Commit = Win32MemoryCommit;
    result.Decommit = Win32MemoryDecommit;
    result.Release = Win32MemoryRelease;
    return result;
}

void *PlatformAllocMemory(size_t size)
{
    void *result = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return result;
}

void PlatformReleaseMemory(void *memory, size_t size)
{
    // TODO: Remove size parameter from this funtion
    (void)size;
    VirtualFree(memory, 0, MEM_RELEASE);
}

FileRes PlatformReadFile(char *fileName)
{
    FileRes result = {0};
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, 
                                              FILE_SHARE_READ, 0, 
                                              OPEN_EXISTING, 
                                              FILE_ATTRIBUTE_NORMAL, 0);
    Assert(fileHandle != INVALID_HANDLE_VALUE);
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize = {0};
        GetFileSizeEx(fileHandle, &fileSize);
        result.size = fileSize.QuadPart;
        result.data = VirtualAlloc(0, result.size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        ReadFile(fileHandle, result.data, (DWORD)result.size, 0, 0);
    }
    else
    {
        // TODO: Good loggin system
    }
    CloseHandle(fileHandle);
    return result;
}

void PlatformFreeFile(FileRes *fileRes)
{
    VirtualFree(fileRes->data, 0, MEM_RELEASE);
    fileRes->data = 0;
    fileRes->size = 0;
}

// NOTE: Win32 Code

// NOTE: Globals variables
static B8 globalRunning;
static Win32BackBuffer globalBackBuffer;
static HDC globalWindowDC;

void Win32CreateBackBuffer(HWND window, HDC deviceContext, Win32BackBuffer *buffer)
{
    // NOTE: Get the actual size of the renderable window
    RECT windowRect = {0};
    GetClientRect(window, &windowRect);
    buffer->width = windowRect.right - windowRect.left;
    buffer->height = windowRect.bottom - windowRect.top;
    // NOTE: Init the bitmap info
    buffer->bitmapInfo.bmiHeader.biSize = sizeof(buffer->bitmapInfo.bmiHeader);
    buffer->bitmapInfo.bmiHeader.biWidth = buffer->width;
    buffer->bitmapInfo.bmiHeader.biHeight = -(I32)(buffer->height);
    buffer->bitmapInfo.bmiHeader.biPlanes = 1;
    buffer->bitmapInfo.bmiHeader.biBitCount = 32;
    buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;

    buffer->bitmap = 
        CreateDIBSection(deviceContext, &buffer->bitmapInfo, DIB_RGB_COLORS, (void **)&(buffer->pixels), 0, 0);
    
    // NOTE: Create a compatible device context to be able to blit it on window
    buffer->bufferDC = CreateCompatibleDC(deviceContext);
    SelectObject(buffer->bufferDC, buffer->bitmap);

    // NOTE: Alloc memory for the z buffer
    size_t zBufferSize = (buffer->width*buffer->height*sizeof(F32));
    buffer->zBuffer = (F32 *)VirtualAlloc(0, zBufferSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

void Win32BlitBackBuffer(HDC destDC, Win32BackBuffer *buffer)
{
    BitBlt(destDC, 0, 0, buffer->width, buffer->height, buffer->bufferDC, 0, 0, SRCCOPY);
}

void Win32ClearBackBuffer(Win32BackBuffer *buffer, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    for(U32 index = 0; index < buffer->width*buffer->height; ++index)
    {
        buffer->pixels[index] = color;
    }
}

void Win32DrawPixel(Win32BackBuffer *buffer, U32 x, U32 y, U8 red, U8 green, U8 blue)
{
    U32 color = ((U32)red << 16)|((U32)green << 8)|((U32)blue << 0);
    buffer->pixels[y * buffer->width + x] = color;
}

LRESULT CALLBACK 
Win32WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_CREATE:
        {
            // NOTE: Init the device context used by the entired aplication
            globalWindowDC = GetDC(window);
            // NOTE: Create a DIB section to render graphics
            Win32CreateBackBuffer(window, globalWindowDC, &globalBackBuffer);
        }break;
        case WM_CLOSE:
        case WM_DESTROY:
        {
            globalRunning = false;
        }break;
        default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
        }break;
    }
    return result;
}

#if 1 
int main()
#else
INT WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#endif
{
    WNDCLASSA windowClass = {0};
    windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    windowClass.lpfnWndProc = Win32WindowProc;
    windowClass.hInstance = 0/*hInstance*/;
    windowClass.lpszClassName = "tlibWindowClass";

    RegisterClassA(&windowClass);

    HWND window = CreateWindowExA(0, windowClass.lpszClassName, "tlibWindow",
                                  WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  WINDOW_WIDTH, WINDOW_HEIGHT,
                                  0, 0, 0/*hInstance*/, 0);

    // NOTE: Pass the backbuffer into the game
    BackBuffer backBuffer = {0};
    backBuffer.pixels = globalBackBuffer.pixels;
    backBuffer.width = globalBackBuffer.width;
    backBuffer.height = globalBackBuffer.height;
    backBuffer.zBuffer = globalBackBuffer.zBuffer; 
    
    GameInit(&backBuffer);
   
    globalRunning = true;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER lastTime;
    QueryPerformanceCounter(&lastTime);

    F64 secondsPerFrame = 1.0/30.0;
    
    while(globalRunning)
    {
        MSG message;
        while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message); 
            DispatchMessageA(&message); 
        }
        
        GameUpdateAndRender(&backBuffer, (F32)secondsPerFrame);
        Win32BlitBackBuffer(globalWindowDC, &globalBackBuffer);
        
        // NOTE: Fix game time step
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        
        F64 frameTime = (F64)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        if(frameTime < secondsPerFrame)
        {
            F64 sleepTime = (secondsPerFrame - frameTime) * 1000.0;
            Sleep((DWORD)sleepTime);
        }
        QueryPerformanceCounter(&currentTime);
#if 0
        frameTime = (F64)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        printf("sec:%lf FPS:%d\n", frameTime, (U32)(1.0/frameTime));
#endif
        lastTime = currentTime;
    }
    
    return 0;
}
