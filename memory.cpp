#include "shared.h"

void* stackPushSize(MemStack* stack, u64 size)
{
    void* end = (char*)stack->start + stack->size;
    if(stack->pointer + size <= end)
    {
        char* ret = stack->pointer;
        stack->pointer += size;
        return (void*)ret;
    }
    else
        return 0;
}

void stackInit(MemStack* stack, void* start, u64 size)
{
    stack->start = (char*)start;
    stack->size = size;
    stack->pointer = stack-> start;
}
