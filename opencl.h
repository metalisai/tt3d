#ifndef OPENCL_H
#define OPENCL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 1
#include <CL/cl.h>
#include <stdio.h>
#include "shared.h"
#include "engine_platform.h"

void checkErr(cl_int err, const char * name) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "ERROR: %s %d\n", name, err);
        exit(EXIT_FAILURE);
        assert(false);
    }
}

struct OpenclState
{
    cl_context context;
    cl_uint deviceIdCount;
    cl_device_id* deviceIds;
} OpenclState;

cl_kernel clCreteKernel(struct OpenclState* clstate, const char* kernelFile)
{
    cl_int error;

    PlatformFileHandle fh = Platform.openFile(kernelFile);
    if(fh.noErrors)
    {
        size_t size = (size_t)Platform.getFileSize(&fh);
        void* fmem = malloc(size);
        Platform.readFromFile(&fh, 0, size, fmem);

        // build kernel

        cl_program program = clCreateProgramWithSource (clstate->context, 1, (const char**)&fmem, (size_t*)&size, &error);
        checkErr(error,"clCreateProgramWithSource");

        checkErr(clBuildProgram (program, clstate->deviceIdCount, clstate->deviceIds, 0, 0, 0), "cBuildProgram");

        cl_kernel kernel = clCreateKernel (program, "SAXPY", &error);
        checkErr (error, "clCreateKernel");

        free(fmem);

        printf("Kernel file loaded!\n");
        return kernel;
    }
    else
    {
        printf("Failed to open kernel file %s\n",kernelFile);
        INVALID_CODE_PATH
        cl_kernel kernel;
        return kernel;
    }
}


u32 inticl(void)
{
    struct OpenclState clstate;

    cl_uint platformIdCount = 0;
    clGetPlatformIDs (0, 0, &platformIdCount);

    cl_platform_id* platformIds = (cl_platform_id*)malloc(sizeof(cl_platform_id)*platformIdCount);
    clGetPlatformIDs (platformIdCount, platformIds, 0);

    clGetDeviceIDs (platformIds [0], CL_DEVICE_TYPE_ALL, 0, 0, &clstate.deviceIdCount);
    clstate.deviceIds = (cl_device_id*) malloc(sizeof(cl_device_id)*clstate.deviceIdCount);
    clGetDeviceIDs (platformIds [0], CL_DEVICE_TYPE_ALL, clstate.deviceIdCount, clstate.deviceIds, 0);

    const cl_context_properties contextProperties [] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)(platformIds [0]),
        0, 0
    };

    cl_int error;
    clstate.context = clCreateContext (contextProperties, clstate.deviceIdCount, clstate.deviceIds, 0, 0, &error);
    checkErr(error, "create context");

    for(int i = 0; i < platformIdCount; i++)
    {
        size_t size = 0;
        clGetPlatformInfo (platformIds[i], CL_PLATFORM_NAME, 0, 0, &size);

        char* result = (char*)malloc(size);
        clGetPlatformInfo (platformIds[i], CL_PLATFORM_NAME, size, result, 0);
        printf("platform %s\n", result);
        free(result);
    }

    for(int i = 0; i < clstate.deviceIdCount; i++)
    {
        size_t size;
        clGetDeviceInfo(clstate.deviceIds[i], CL_DEVICE_NAME, 0, 0, &size);
        char* result = (char*)malloc(size);
        clGetDeviceInfo(clstate.deviceIds[i], CL_DEVICE_NAME, size, result, 0);
        printf("device %s\n", result);
        free(result);
    }

    size_t testDataSize = 1 << 10;
    float a[testDataSize]; // c99 heheeh xd
    float b[testDataSize];
    for (int i = 0; i < testDataSize; ++i) {
        a [i] = (r32) (23 ^ i);
        b [i] = (r32) (42 ^ i);
    }

    cl_mem aBuffer = clCreateBuffer (clstate.context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * testDataSize,
        a, &error);
    checkErr(error, "clCreateBuffer");

    cl_mem bBuffer = clCreateBuffer (clstate.context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof (float) * testDataSize,
        b, &error);
    checkErr (error, "clCreateBuffer");

    char* kernelFile = "kernels/saxpy.cl";
    cl_kernel kernel = clCreteKernel(&clstate, kernelFile);

    clSetKernelArg (kernel, 0, sizeof (cl_mem), &aBuffer);
    clSetKernelArg (kernel, 1, sizeof (cl_mem), &bBuffer);
    static const float two = 2.0f;
    clSetKernelArg (kernel, 2, sizeof (float), &two);
    const size_t globalWorkSize [] = { testDataSize, 0, 0 };

    cl_command_queue queue = clCreateCommandQueue (clstate.context, clstate.deviceIds [0],
            0, &error);
        checkErr (error,"clCreateCommandQueue");

    for(int i = 0; i < testDataSize; i++)
    {
        printf("before: %f \n", b[i]);
    }

    clEnqueueNDRangeKernel (queue, kernel,
        1, // One dimension
        0,
        globalWorkSize,
        0,
        0, 0, 0);

    clEnqueueReadBuffer (queue, bBuffer, CL_TRUE, 0,
        sizeof (float) * testDataSize,
        b,
        0, 0, 0);

    for(int i = 0; i < testDataSize; i++)
    {
        printf("after: %f \n", b[i]);
    }


    clReleaseCommandQueue (queue);

    clReleaseMemObject (bBuffer);
    clReleaseMemObject (aBuffer);

    clReleaseKernel (kernel);
    //clReleaseProgram (program); // TODO

    clReleaseContext (clstate.context);

    free(platformIds);
    free(clstate.deviceIds);

    return 0;
}

#endif // OPENCL_H
