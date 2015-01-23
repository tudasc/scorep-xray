/* *INDENT-OFF* */

cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCreateSubDevices ) ( cl_device_id                        inDevice,
                                                 const cl_device_partition_property* properties,
                                                 cl_uint                             numDevices,
                                                 cl_device_id*                       outDevices,
                                                 cl_uint*                            numDevicesRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateSubDevices );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateSubDevices,
                                             clCreateSubDevices,
                                             ( inDevice, properties, numDevices,
                                               outDevices, numDevicesRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateSubDevices );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainDevice ) ( cl_device_id device )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainDevice );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainDevice,
                                             clRetainDevice,
                                             ( device ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainDevice );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseDevice ) ( cl_device_id device )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseDevice );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseDevice,
                                             clReleaseDevice,
                                             ( device ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseDevice );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage ) ( cl_context             context,
                                            cl_mem_flags           flags,
                                            const cl_image_format* imageFormat,
                                            const cl_image_desc*   imageDesc,
                                            void*                  hostPtr,
                                            cl_int*                errCodeRet )
{
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateImage,
                                             clCreateImage,
                                             ( context, flags, imageFormat,
                                               imageDesc, hostPtr, errCodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateImage );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithBuiltInKernels ) ( cl_context          context,
                                                                cl_uint             numDevices,
                                                                const cl_device_id* deviceList,
                                                                const char*         kernelNames,
                                                                cl_int*             errCodeRet )
{
    cl_program ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateProgramWithBuiltInKernels );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateProgramWithBuiltInKernels,
                                             clCreateProgramWithBuiltInKernels,
                                             ( context, numDevices, deviceList,
                                               kernelNames, errCodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateProgramWithBuiltInKernels );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCompileProgram ) ( cl_program          program,
                                               cl_uint             numDevices,
                                               const cl_device_id* deviceList,
                                               const char*         options,
                                               cl_uint             numInputHeaders,
                                               const cl_program*   inputHeaders,
                                               const char**        headerIncludeNames,
                                               void ( CL_CALLBACK* pfnNotify )( cl_program,
                                                                                void* ),
                                               void*               userData )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCompileProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCompileProgram,
                                             clCompileProgram,
                                             ( program, numDevices, deviceList,
                                               options, numInputHeaders,
                                               inputHeaders, headerIncludeNames,
                                               pfnNotify, userData ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCompileProgram );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clLinkProgram ) ( cl_context          context,
                                            cl_uint             numDevices,
                                            const cl_device_id* deviceList,
                                            const char*         options,
                                            cl_uint             numInputPrograms,
                                            const cl_program*   inputPrograms,
                                            void ( CL_CALLBACK* pfnNotify )( cl_program,
                                                                             void* ),
                                            void*               userData,
                                            cl_int*             errCodeRet )
{
    cl_program ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clLinkProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clLinkProgram,
                                             clLinkProgram,
                                             ( context, numDevices, deviceList,
                                               options, numInputPrograms,
                                               inputPrograms, pfnNotify, userData,
                                               errCodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clLinkProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clUnloadPlatformCompiler ) ( cl_platform_id platform )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clUnloadPlatformCompiler );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clUnloadPlatformCompiler,
                                             clUnloadPlatformCompiler,
                                             ( platform ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clUnloadPlatformCompiler );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelArgInfo ) ( cl_kernel          kernel,
                                                 cl_uint            argIndex,
                                                 cl_kernel_arg_info paramName,
                                                 size_t             paramValueSize,
                                                 void*              paramValue,
                                                 size_t*            paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetKernelArgInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetKernelArgInfo,
                                             clGetKernelArgInfo,
                                             ( kernel, argIndex, paramName,
                                               paramValueSize, paramValue,
                                               paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetKernelArgInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueFillBuffer ) ( cl_command_queue commandQueue,
                                                  cl_mem           buffer,
                                                  const void*      pattern,
                                                  size_t           patternSize,
                                                  size_t           offset,
                                                  size_t           size,
                                                  cl_uint          numEventsInWaitList,
                                                  const cl_event*  eventWaitList,
                                                  cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueFillBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueFillBuffer,
                                             clEnqueueFillBuffer,
                                             ( commandQueue, buffer, pattern,
                                               patternSize, offset, size,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueFillBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueFillImage ) ( cl_command_queue command_queue,
                                                 cl_mem           image,
                                                 const void*      fillColor,
                                                 const size_t*    origin,
                                                 const size_t*    region,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueFillImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueFillImage,
                                             clEnqueueFillImage,
                                             ( command_queue, image, fillColor,
                                               origin, region, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueFillImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMigrateMemObjects ) ( cl_command_queue       commandQueue,
                                                         cl_uint                numMemObjects,
                                                         const cl_mem*          memObjects,
                                                         cl_mem_migration_flags flags,
                                                         cl_uint                numEventsInWaitList,
                                                         const cl_event*        eventWaitList,
                                                         cl_event*              event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueMigrateMemObjects );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueMigrateMemObjects,
                                             clEnqueueMigrateMemObjects,
                                             ( commandQueue, numMemObjects,
                                               memObjects, flags,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueMigrateMemObjects );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMarkerWithWaitList ) ( cl_command_queue commandQueue,
                                                          cl_uint          numEventsInWaitList,
                                                          const cl_event*  eventWaitList,
                                                          cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueMarkerWithWaitList );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueMarkerWithWaitList,
                                             clEnqueueMarkerWithWaitList,
                                             ( commandQueue, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueMarkerWithWaitList );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueBarrierWithWaitList ) ( cl_command_queue commandQueue,
                                                           cl_uint          numEventsInWaitList,
                                                           const cl_event*  eventWaitList,
                                                           cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueBarrierWithWaitList );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueBarrierWithWaitList,
                                             clEnqueueBarrierWithWaitList,
                                             ( commandQueue, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueBarrierWithWaitList );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clGetExtensionFunctionAddressForPlatform ) ( cl_platform_id platform,
                                                                       const char*    funcName )
{
    void* ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetExtensionFunctionAddressForPlatform );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetExtensionFunctionAddressForPlatform,
                                             clGetExtensionFunctionAddressForPlatform,
                                             ( platform, funcName ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetExtensionFunctionAddressForPlatform );

    return ret;
}

/* *INDENT-ON* */
