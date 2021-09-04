#include "tlib_main.cpp"
#include "tlib_render.cpp"

#include "tlib_types.h"
#include "tlib_math.h"
#include "tlib_win32.h"

#define WINDOW_WIDTH 800 
#define WINDOW_HEIGHT 600

// NOTE: Globals variables
static B8 globalRunning;
static Win32BackBuffer globalBackBuffer;
static HDC globalWindowDC;

// NOTE: Functions to the client
void GameInit(BackBuffer *backBuffer);
void GameUpdateAndRender(BackBuffer *backBuffer, F32 dt);

void Win32CreateBackBuffer(HWND window, HDC deviceContext, Win32BackBuffer *buffer)
{
    // NOTE: Get the actual size of the renderable window
    RECT windowRect = {};
    GetClientRect(window, &windowRect);
    buffer->width = windowRect.right - windowRect.left;
    buffer->height = windowRect.bottom - windowRect.top;
    // NOTE: Init the bitmap info
    buffer->bitmapInfo.bmiHeader.biSize = sizeof(buffer->bitmapInfo.bmiHeader);
    buffer->bitmapInfo.bmiHeader.biWidth = buffer->width;
    buffer->bitmapInfo.bmiHeader.biHeight = -buffer->height;
    buffer->bitmapInfo.bmiHeader.biPlanes = 1;
    buffer->bitmapInfo.bmiHeader.biBitCount = 32;
    buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;

    buffer->bitmap = CreateDIBSection(deviceContext, &buffer->bitmapInfo, DIB_RGB_COLORS, &((void *)buffer->pixels), 0, 0);
    
    // NOTE: Create a compatible device context to be able to blit it on window
    buffer->bufferDC = CreateCompatibleDC(deviceContext);
    SelectObject(buffer->bufferDC, buffer->bitmap);
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

// TODO: Remove star field from the platfom layer

void StarFieldInit(StarField *starField)
{
    for(I32 index = 0; index < ArrayCount(starField->pos); ++index)
    {
        // NOTE: The coordinates are normallize to -1 to 1
        starField->pos[index].x = (RandomF32() - 0.5f) * 2;
        starField->pos[index].y = (RandomF32() - 0.5f) * 2;
        starField->pos[index].z = (RandomF32() - 0.5f) * 2;
        
        // NOTE: Color are from 0 to 1 
        starField->color[index].x = RandomF32();
        starField->color[index].y = RandomF32();
        starField->color[index].z = RandomF32();
    }
}

void StarFieldUpdateAndRender(Win32BackBuffer *buffer, F32 dt, StarField *starField)
{
    // NOTE: Project coordinate to the buffer width and height
    U32 halfBufferWidht = 0.5f*buffer->width;
    U32 halfBufferHeight = 0.5f*buffer->height;
    for(I32 index = 0; index < ArrayCount(starField->pos); ++index)
    {
        // NOTE: Update Star
        starField->pos[index].z -= dt;

        // NOTE: Render Star
        if(starField->pos[index].z <= 0)
        {
            starField->pos[index].x = (RandomF32() - 0.5f) * 2;
            starField->pos[index].y = (RandomF32() - 0.5f) * 2;
            starField->pos[index].z = (RandomF32() - 0.5f) * 2;
        }
        else
        {
            // NOTE: Project Star into screen space
            F32 z = starField->pos[index].z;
            I32 screenX = (starField->pos[index].x/z * halfBufferWidht) + halfBufferWidht;
            I32 screenY = (starField->pos[index].y/z * halfBufferHeight) + halfBufferHeight;
            
            if(screenX < 0 || screenX >= buffer->width || 
               screenY < 0 || screenY >= buffer->height)
            {
                starField->pos[index].x = (RandomF32() - 0.5f) * 2;
                starField->pos[index].y = (RandomF32() - 0.5f) * 2;
                starField->pos[index].z = (RandomF32() - 0.5f) * 2;
            }
            else
            {
                // NOTE: Map color from 0 to 1 to 127 to 255
                U8 red =   (U8)(starField->color[index].x * 127 + 128);
                U8 green = (U8)(starField->color[index].y * 127 + 128);
                U8 blue =  (U8)(starField->color[index].z * 127 + 128);
                Win32DrawPixel(buffer, screenX, screenY, red, green, blue); 
            }
        }
    }
}

INT WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    windowClass.lpfnWndProc = Win32WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "tlibWindowClass";

    RegisterClassA(&windowClass);

    HWND window = CreateWindowExA(0, windowClass.lpszClassName, "tlibWindow",
                                  WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  WINDOW_WIDTH, WINDOW_HEIGHT,
                                  0, 0, hInstance, 0);

    // NOTE: Pass the backbuffer into the game
    BackBuffer backBuffer = {};
    backBuffer.pixels = globalBackBuffer.pixels;
    backBuffer.width = globalBackBuffer.width;
    backBuffer.height = globalBackBuffer.height;
    
    StarField starField;
    StarFieldInit(&starField);
    GameInit(&backBuffer);
   
    globalRunning = true;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER lastTime;
    QueryPerformanceCounter(&lastTime);

    while(globalRunning)
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        
        I64 deltaTime = currentTime.QuadPart - lastTime.QuadPart;
        F32 dt = ((F32)deltaTime / (F32)frequency.QuadPart);
        lastTime = currentTime;

        MSG message;
        while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message); 
            DispatchMessageA(&message); 
        }
        
        Win32ClearBackBuffer(&globalBackBuffer, 0x00, 0x00, 0x00);

        StarFieldUpdateAndRender(&globalBackBuffer, dt, &starField);
        GameUpdateAndRender(&backBuffer, dt);

        Win32BlitBackBuffer(globalWindowDC, &globalBackBuffer);
    }
    
    return 0;
}
