#ifndef DARRAY_H
#define DARRAY_H

#define DArraySizePtr(dArray) (((size_t *)dArray) - 2)
#define DArrayCapPtr(dArray)  (((size_t *)dArray) - 1)

void *PushDArray(void *dArray, size_t size)
{
    if(!dArray)
    {
        size_t capacity = (sizeof(size_t)*2)+(size*2);
        dArray = PlatformAllocMemory(capacity);
        dArray = ((U8 *)dArray) + (sizeof(size_t)*2);

        *DArrayCapPtr(dArray) = capacity;
        *DArraySizePtr(dArray) = size;
    }
    

    return ((U8 *)dArray) + *DArraySizePtr(dArray);
}

#endif // DARRAY_H
