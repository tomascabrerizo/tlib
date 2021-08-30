#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float r32;
typedef uint8_t  b8;
typedef uint32_t b32;

#define align_64(value) (((value) + 0x7) & ~0x7)
#define align_32(value) (((value) + 0x3) & ~0x3)

#define array_count(array) (sizeof(array)/sizeof(array[0]))

#define rle_newline(length) ((u32)(0x00000000 | (u16)(length)))
#define rle_skiprun(length) ((u32)(0x00010000 | (u16)(length)))
#define rle_copyrun(length) ((u32)(0x00020000 | (u16)(length)))

#define rle_lenght(word) ((u32)(word & 0xFFFF))

enum Rle_State
{
    in_skiprun,
    in_copyrun,
};

void compress_u8sprite(u8 *output_buffer, u8 *source, u32 width, u32 heihgt, u8 transparent_color)
{
    
    u32 *output_record = (u32 *)output_buffer;
    
    u8 *scan_line = source;
    for(u32 y = 0; y < heihgt; ++y)
    {
        u32 line_lenght = 4;
        u32 current_run_lenght = 1;

        u32 *new_line_record = output_record;
        output_record += 1;
        u8 *output_byte = (u8 *)(output_record + 1);
        
        u8 *pixel = scan_line;
        
        // NOTE: Init the states
        Rle_State state;
        if(*pixel == transparent_color)
        {
            state = in_skiprun;
            line_lenght += 4;
        }
        else
        {
            state = in_copyrun;
            *output_byte++ = *pixel;
            line_lenght += 5;
        }
        
        ++pixel;
        
        for(u32 x = 1; x < width; ++x)
        {
            if(*pixel == transparent_color)
            {
                if(state == in_skiprun)
                {
                    ++current_run_lenght;
                }
                else
                {
                    // NOTE: Write copyrun and change state to skiprun
                    *output_record = rle_copyrun(current_run_lenght);
                    output_record = (u32 *)output_byte;
                    state = in_skiprun;
                    current_run_lenght = 1;
                    line_lenght += 4;
                }
            }
            else
            {
                if(state == in_copyrun)
                {
                    ++current_run_lenght;
                    *output_byte++ = *pixel;
                    ++line_lenght;
                }
                else
                {
                    // NOTE: Write skiprun and change state to copyrun
                    *output_record = rle_skiprun(current_run_lenght);
                    output_record++;
                    output_byte = (u8 *)(output_record + 1);
                    state = in_copyrun;
                    *output_byte++ = *pixel;
                    current_run_lenght = 1;
                    line_lenght += 5;
                }
            }
            ++pixel;
        }

         
        scan_line += width;
        
        if(state == in_skiprun)
        {
            *output_record = rle_skiprun(current_run_lenght);
            output_record++;
        }
        else
        {
            *output_record = rle_copyrun(current_run_lenght);
            output_record = (u32 *)output_byte;
        }
        *new_line_record = rle_newline(line_lenght);
    }
}

int main(void)
{
    static u8 bitmap[16] = 
    {
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0
    };

    static u8 compress_bitmap[32] = {};
    compress_u8sprite(compress_bitmap, bitmap, 8, 2, 0);
    
    u32 *line1 = (u32 *)compress_bitmap;
    u32 *line2 = (u32 *)(compress_bitmap + *line1);
    
    printf("1 line length %d\n", rle_lenght(*line1));
    printf("2 line length %d\n", rle_lenght(*line2));
    
    return 0;
}

#if 0
INT WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    return 0;
}
#endif
