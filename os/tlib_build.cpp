#include "tlib_types.h"

#include "alpha_blt.cpp"

#include "texture_mapper.h"
#include "texture_mapper.cpp"

#define BUFFER_WIDTH 8 
#define BUFFER_HEIGHT 2

int main(void)
{
    static U8 bitmap[16] = 
    {
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0
    };
    static U8 compress_bitmap[32] = {};
    static U8 buffer[BUFFER_WIDTH*BUFFER_HEIGHT];

    I32 break_here = 0;

    compress_u8sprite(compress_bitmap, bitmap, 8, 2, 0);
    transparent_blt_rle(buffer, BUFFER_WIDTH, BUFFER_HEIGHT, 0, 0, compress_bitmap, 8, 2, 0);
    
    return 0;
}
