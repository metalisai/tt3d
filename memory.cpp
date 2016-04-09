#include "memory.h"

void* stackPushSize(MemStack* stack, u64 size)
{
    if(stack->pointer + size <= stack->end)
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
    stack->end = stack->start + size;
    stack->pointer = stack-> start;
}
