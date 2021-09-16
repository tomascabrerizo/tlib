#ifndef DARRAY_H
#define DARRAY_H

#include <string.h>

#define DArrayCapPtr(dArray)  (((size_t *)(dArray)) - 2)
#define DArraySizePtr(dArray) (((size_t *)(dArray)) - 1)

#define DArrayPush(array, type) (type *)DArrayPush_(array, sizeof(type))
#define DArraySize(array, type) (type *)(DArraySize_(array) / sizeof(type))

size_t DArraySize_(void *dArray)
{
    return *DArraySizePtr(dArray);
}

void *DArrayPush_(void **dArray, size_t size)
{
    if(!*dArray)
    {
        size_t capacity = (sizeof(size_t)*2)+(size*2);
        *dArray = PlatformAllocMemory(capacity);
        *dArray = ((size_t *)(*dArray) + 2);

        *DArrayCapPtr(*dArray) = capacity;
        *DArraySizePtr(*dArray) = size;
        return *dArray;
    }
    else if((*DArraySizePtr(*dArray) + size) >= (*DArrayCapPtr(*dArray)- sizeof(size_t)*2))
    {
        size_t dArraySize = *DArraySizePtr(*dArray);
        void *oldMem = ((size_t *)(*dArray) - 2);
        
        // NOTE: Allocate a bigger array and copy the old in the new one
        size_t newCapacity = (sizeof(size_t)*2)+dArraySize*2;
        void * newDArray = PlatformAllocMemory(newCapacity);
        newDArray = ((size_t *)newDArray + 2);
        
        memcpy(newDArray, *dArray, dArraySize);
        *dArray = newDArray;
        // NOTE: Release the old array
        PlatformReleaseMemory(oldMem, *((size_t *)oldMem));
        
        void *result = (U8 *)(*dArray) + dArraySize;

        // NOTE: Set the new size and capacity
        *DArrayCapPtr(*dArray) = newCapacity;
        *DArraySizePtr(*dArray) = dArraySize + size;
        
        return result;
    }
    else
    {
        void *result = (U8 *)(*dArray) + *DArraySizePtr(*dArray);
        *DArraySizePtr(*dArray) += size;
        return result;
    }
}

void DArrayRelease(void **dArray)
{
    void *mem = ((size_t *)(*dArray) - 2);
    size_t capacity = (*(size_t *)mem);
    PlatformReleaseMemory(mem, capacity);
    *dArray = 0;
}

#endif // DARRAY_H
