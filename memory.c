#include "shared.h"

void* arenaPushSize(MemoryArena* stack, u64 size)
{
    size = size + (size%MEMORYARENA_ALIGNMENT);
    void* end = (char*)stack->start + stack->size;
    if((stack->pointer + size) <= (char*)end)
    {
        char* ret = stack->pointer;
        stack->pointer += size;
        // TODO: WHY ARE IFDEFING ASSERTS MORON
        assert(is_aligned((void*)ret, MEMORYARENA_ALIGNMENT)); // memory not aligned
        return (void*)ret;
    }
    else
        return 0;
}

void createArena(MemoryArena* stack, void* start, u64 size)
{
    assert(is_aligned(start, MEMORYARENA_ALIGNMENT)); // memory not aligned
    stack->start = (char*)start;
    stack->size = size;
    stack->pointer = stack-> start;
}
