#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tlib_types.h"

#define rle_newline(length) ((U32)(0x00000000 | (U16)(length)))
#define rle_skiprun(length) ((U32)(0x00010000 | (U16)(length)))
#define rle_copyrun(length) ((U32)(0x00020000 | (U16)(length)))

#define rle_lenght(word) ((U32)(word & 0xFFFF))

#define is_skiprun(word) (((U32)word & 0xFFFF0000) == 0x00010000)
#define is_copyrun(word) (((U32)word & 0xFFFF0000) == 0x00020000)
#define run_length(word) ((U32)(word & 0xFFFF))

enum Rle_State
{
    in_skiprun,
    in_copyrun,
};

void compress_u8sprite(U8 *output_buffer, U8 *source, U32 width, U32 height, U8 transparent_color)
{
    U32 *output_record = (U32 *)output_buffer;
    
    U8 *scan_line = source;
    for(U32 y = 0; y < height; ++y)
    {
        U32 line_lenght = 4;
        U32 current_run_lenght = 1;

        U32 *new_line_record = output_record;
        output_record += 1;
        U8 *output_byte = (U8 *)(output_record + 1);
        
        U8 *pixel = scan_line;
        
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
        
        for(U32 x = 1; x < width; ++x)
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
                    output_record = (U32 *)output_byte;
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
                    output_byte = (U8 *)(output_record + 1);
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
            output_record = (U32 *)output_byte;
        }
        *new_line_record = rle_newline(line_lenght);
    }
}

void transparent_blt_rle(U8 *buffer, int buffer_width, int buffer_height, int x, int y, 
                         U8 *source, int source_width, int source_height, U8 transparent_color)
{
    U32 x_source = 0;
    U32 y_source = 0;

    int width = source_width;
    int height = source_height;
    if(x < 0)
    {
        width += x;
        x_source -= x;
        x = 0;
    }
    if((x + width) > buffer_width)
    {
        width = (buffer_width - x);
    }
    if(y < 0)
    {
        height += y;
        y_source -= y;
        y = 0;
    }
    if((y + height) > buffer_height)
    {
        height = (buffer_height - y);
    }
    
    U8 *buffer_line =  buffer + (y * buffer_width + x);

    if((height > 0) && (width > 0))
    {
        int x = 0;
        int y = 0;
        U32 *current_source_scan = (U32 *)source;

        // NOTE: Prestep the source y
        for(y = 0; y < y_source; ++y)
        {
            current_source_scan = (U32 *)((U8 *)current_source_scan + run_length(*current_source_scan));
        }

        for(y = 0; y < height; ++y)
        {
            U8 *buffer_pixels = buffer_line;
            U32 *current_source_record = current_source_scan + 1;

            // NOTE: Prestep the source x
            x = 0;
            while(x < x_source)
            {
                x += run_length(*current_source_record);

                if(x > x_source)
                {
                    // NOTE: We need to partially process the current record
                    int overlap = x_source - x;
                    int active_overlap = overlap > width ? width : overlap;

                    if(is_copyrun(*current_source_record))
                    {
                        // NOTE: Copy overlap pixel to buffer 
                        // NOTE: Get pointer to data
                        U8 *copy_run = (U8 *)current_source_record + 4;
                        // NOTE: Prestep to desired pixels
                        copy_run += run_length(*current_source_record) - overlap;
                        memcpy(buffer_pixels, copy_run, active_overlap);
                    }
                    // NOTE: Skip to the next dest pixel
                    buffer_pixels += active_overlap;
                }

                // Skip to next report
                if(is_copyrun(*current_source_record))
                {
                    current_source_record = (U32 *)((U8 *)current_source_record + run_length(*current_source_record));
                }
                ++current_source_record;
            }

            x = x - x_source;

            while(x < width)
            {
                int length = run_length(*current_source_record);
                int remaining_width = width - x;
                int active_pixels = (length > remaining_width ? remaining_width : length);

                if(is_copyrun(*current_source_record))
                {
                    U8 *copy_run = (U8 *)current_source_record + 4;
                    memcpy(buffer_pixels, copy_run, active_pixels);
                }
                // NOTE: Skip to the next dest pixel
                buffer_pixels += active_pixels;

                // Skip to next report
                if(is_copyrun(*current_source_record))
                {
                    current_source_record = (U32 *)((U8 *)current_source_record + length);
                }
                ++current_source_record;
                x += length;
            }

            buffer_line += buffer_width;
            current_source_scan = (U32 *)((U8 *)current_source_scan + run_length(*current_source_scan));
        }
    }
}
