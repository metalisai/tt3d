#ifndef ENGINE_PLATFORM_H
#define ENGINE_PLATFORM_H

#include "shared.h"

//#define internal static

typedef struct PlatformFileHandle
{
    b32 noErrors;
    void* PlatformData;
} PlatformFileHandle;

#define PLATFORM_OPEN_FILE(name) PlatformFileHandle name(const char *fileName)
typedef PLATFORM_OPEN_FILE(platformOpenFile);

#define PLATFORM_GET_FILE_SIZE(name) i64 name(PlatformFileHandle *fh)
typedef PLATFORM_GET_FILE_SIZE(platformGetFileSize);

#define PLATFORM_READ_DATA_FROM_FILE(name) void name(PlatformFileHandle *source, i64 offset, u64 size, void *dest)
typedef PLATFORM_READ_DATA_FROM_FILE(platformReadFromFile);

#define PLATFORM_CLOSE_FILE(name) void name(PlatformFileHandle *handle)
typedef PLATFORM_CLOSE_FILE(platformCloseFile);

typedef struct PlatformApi
{
    platformOpenFile *openFile;
    platformGetFileSize *getFileSize;
    platformReadFromFile *readFromFile;
    platformCloseFile *closeFile;
} PlatformApi;
extern PlatformApi Platform;

typedef struct EngineMemory
{
    struct GameState *gameState;
    struct TransientState *transientState;

    u64 permanentStorageSize;
    u64 transientStorageSize;

    PlatformApi platformApi;
} EngineMemory;

#endif // ENGINE_PLATFORM_H
