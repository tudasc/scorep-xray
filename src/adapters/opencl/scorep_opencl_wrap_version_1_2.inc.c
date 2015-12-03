cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCreateSubDevices )( cl_device_id                        inDevice,
                                                const cl_device_partition_property* properties,
                                                cl_uint                             numDevices,
                                                cl_device_id*                       outDevices,
                                                cl_uint*                            numDevicesRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateSubDevices );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clCreateSubDevices, clCreateSubDevices,
        ( inDevice, properties, numDevices, outDevices, numDevicesRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateSubDevices );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainDevice )( cl_device_id device )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainDevice );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clRetainDevice, clRetainDevice, ( device ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainDevice );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseDevice )( cl_device_id device )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseDevice );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clReleaseDevice, clReleaseDevice, ( device ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseDevice );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage )( cl_context             context,
                                           cl_mem_flags           flags,
                                           const cl_image_format* imageFormat,
                                           const cl_image_desc*   imageDesc,
                                           void*                  hostPtr,
                                           cl_int*                errCodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clCreateImage, clCreateImage,
        ( context, flags, imageFormat, imageDesc, hostPtr, errCodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateImage );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithBuiltInKernels )( cl_context          context,
                                                               cl_uint             numDevices,
                                                               const cl_device_id* deviceList,
                                                               const char*         kernelNames,
                                                               cl_int*             errCodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateProgramWithBuiltInKernels );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_program ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clCreateProgramWithBuiltInKernels, clCreateProgramWithBuiltInKernels,
        ( context, numDevices, deviceList, kernelNames, errCodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateProgramWithBuiltInKernels );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCompileProgram )( cl_program          program,
                                              cl_uint             numDevices,
                                              const cl_device_id* deviceList,
                                              const char*         options,
                                              cl_uint             numInputHeaders,
                                              const cl_program*   inputHeaders,
                                              const char**        headerIncludeNames,
                                              void( CL_CALLBACK * pfnNotify )( cl_program,
                                                                               void* ),
                                              void*               userData )
{
    SCOREP_OPENCL_FUNC_ENTER( clCompileProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clCompileProgram, clCompileProgram,
        ( program, numDevices, deviceList, options, numInputHeaders,
          inputHeaders, headerIncludeNames, pfnNotify, userData ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCompileProgram );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clLinkProgram )( cl_context          context,
                                           cl_uint             numDevices,
                                           const cl_device_id* deviceList,
                                           const char*         options,
                                           cl_uint             numInputPrograms,
                                           const cl_program*   inputPrograms,
                                           void( CL_CALLBACK * pfnNotify )( cl_program,
                                                                            void* ),
                                           void*               userData,
                                           cl_int*             errCodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clLinkProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_program ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clLinkProgram, clLinkProgram,
        ( context, numDevices, deviceList, options, numInputPrograms,
          inputPrograms, pfnNotify, userData, errCodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clLinkProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clUnloadPlatformCompiler )( cl_platform_id platform )
{
    SCOREP_OPENCL_FUNC_ENTER( clUnloadPlatformCompiler );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clUnloadPlatformCompiler, clUnloadPlatformCompiler,
        ( platform ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clUnloadPlatformCompiler );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelArgInfo )( cl_kernel          kernel,
                                                cl_uint            argIndex,
                                                cl_kernel_arg_info paramName,
                                                size_t             paramValueSize,
                                                void*              paramValue,
                                                size_t*            paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetKernelArgInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clGetKernelArgInfo, clGetKernelArgInfo,
        ( kernel, argIndex, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetKernelArgInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueFillBuffer )( cl_command_queue commandQueue,
                                                 cl_mem           buffer,
                                                 const void*      pattern,
                                                 size_t           patternSize,
                                                 size_t           offset,
                                                 size_t           size,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueFillBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clEnqueueFillBuffer, clEnqueueFillBuffer,
        ( commandQueue, buffer, pattern, patternSize, offset, size,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueFillBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueFillImage )( cl_command_queue command_queue,
                                                cl_mem           image,
                                                const void*      fillColor,
                                                const size_t*    origin,
                                                const size_t*    region,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueFillImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clEnqueueFillImage, clEnqueueFillImage,
        ( command_queue, image, fillColor, origin, region, numEventsInWaitList,
          eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueFillImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMigrateMemObjects )( cl_command_queue       commandQueue,
                                                        cl_uint                numMemObjects,
                                                        const cl_mem*          memObjects,
                                                        cl_mem_migration_flags flags,
                                                        cl_uint                numEventsInWaitList,
                                                        const cl_event*        eventWaitList,
                                                        cl_event*              event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueMigrateMemObjects );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clEnqueueMigrateMemObjects, clEnqueueMigrateMemObjects,
        ( commandQueue, numMemObjects, memObjects, flags, numEventsInWaitList,
          eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueMigrateMemObjects );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMarkerWithWaitList )( cl_command_queue commandQueue,
                                                         cl_uint          numEventsInWaitList,
                                                         const cl_event*  eventWaitList,
                                                         cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueMarkerWithWaitList );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clEnqueueMarkerWithWaitList, clEnqueueMarkerWithWaitList,
        ( commandQueue, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueMarkerWithWaitList );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueBarrierWithWaitList )( cl_command_queue commandQueue,
                                                          cl_uint          numEventsInWaitList,
                                                          const cl_event*  eventWaitList,
                                                          cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueBarrierWithWaitList );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clEnqueueBarrierWithWaitList, clEnqueueBarrierWithWaitList,
        ( commandQueue, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueBarrierWithWaitList );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clGetExtensionFunctionAddressForPlatform )( cl_platform_id platform,
                                                                      const char*    funcName )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetExtensionFunctionAddressForPlatform );

    SCOREP_OPENCL_WRAP_ENTER();
    void* ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL(
        scorep_opencl_funcptr_clGetExtensionFunctionAddressForPlatform,
        clGetExtensionFunctionAddressForPlatform,
        ( platform, funcName ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetExtensionFunctionAddressForPlatform );

    return ret;
}
