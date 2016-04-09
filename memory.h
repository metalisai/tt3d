#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

struct MemStack
{
    char* start;
    char* pointer;
    char* end;
    u64 size;
};

void* stackPushSize(MemStack* stack, u64 size);
void stackInit(MemStack* stack, void* start, u64 size);

#endif // MEMORY_H
