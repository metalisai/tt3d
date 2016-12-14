#include "renderer.h"

RenderGroupEntryHeader* pushBuffer(RenderGroup* renderGroup, u32 size)
{
    RenderCommands* commands = renderGroup->commands;
    RenderGroupEntryHeader* header = (RenderGroupEntryHeader*)commands->pushBufferDataAt;
    header->size = size;
    // TODO: bounds check
    commands->pushBufferDataAt += size;
    commands->commands++;
    return header;
}

void resetBuffer(RenderGroup* renderGroup)
{
    renderGroup->commands->pushBufferDataAt = renderGroup->commands->pushBufferBase;
    renderGroup->commands->commands = 0;
}

void allocateRenderGroup(MemoryArena* arena, RenderGroup* renderGroup)
{
    RenderCommands* commands = arenaPushSize(arena, sizeof(RenderCommands));
    // TODO: should be left as constant?
    commands->maxPushBufferSize = Megabytes(10);
    u8* commandsBuffer = arenaPushSize(arena, commands->maxPushBufferSize);
    commands->pushBufferBase = commandsBuffer;
    commands->pushBufferDataAt = commandsBuffer;
    //commands->clearColor = vec4(0.0f,0.0f,0.0f,1.0f);
    //commands->width = width;
    //commands->heihgt = height;
    renderGroup->commands = commands;
    //renderGroup->camera = cam;
}

void* pushRenderElement(RenderGroup *group, u32 size, RenderGroupEntryType type)
{
    //RenderCommands* commands = group->commands;
    void* result;

    size += sizeof(RenderGroupEntryHeader);
    RenderGroupEntryHeader* header = pushBuffer(group, size);
    header->type = type;
    result = (u8*)header + sizeof(RenderGroupEntryHeader);
    return result;
}

void pushMesh(RenderGroup *group, Mesh* mesh, Transform* transform, Material material)
{
    MeshEntry* entry = pushRenderElement(group, sizeof(MeshEntry), RenderGroupEntryType_Mesh);
    entry->mesh = mesh;
    entry->transform = transform;
    entry->material = material;
}

void pushArrayMesh(RenderGroup *group, ArrayMesh* mesh, Transform* transform, Material material)
{
    TerrainMeshEntry* entry = pushRenderElement(group, sizeof(TerrainMeshEntry), RenderGroupEntryType_ArrayMesh);
    entry->mesh = mesh;
    entry->transform = transform;
    entry->material = material;
}
