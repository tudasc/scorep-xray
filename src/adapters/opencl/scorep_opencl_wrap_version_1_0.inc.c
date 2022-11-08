#if HAVE( OPENCL_1_0_SYMBOL_CLGETPLATFORMIDS )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPlatformIDs )( cl_uint         numEntries,
                                              cl_platform_id* platforms,
                                              cl_uint*        numPlatforms )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetPlatformIDs );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetPlatformIDs, ( numEntries, platforms, numPlatforms ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetPlatformIDs );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETPLATFORMINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPlatformInfo )( cl_platform_id   platform,
                                               cl_platform_info paramName,
                                               size_t           paramValueSize,
                                               void*            paramValue,
                                               size_t*          paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetPlatformInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetPlatformInfo,
        ( platform, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetPlatformInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETDEVICEIDS )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetDeviceIDs )( cl_platform_id platform,
                                            cl_device_type deviceType,
                                            cl_uint        numEntries,
                                            cl_device_id*  devices,
                                            cl_uint*       numDevices )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetDeviceIDs );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetDeviceIDs,
        ( platform, deviceType, numEntries, devices, numDevices ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetDeviceIDs );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETDEVICEINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetDeviceInfo )( cl_device_id   device,
                                             cl_device_info paramName,
                                             size_t         paramValueSize,
                                             void*          paramValue,
                                             size_t*        paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetDeviceInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetDeviceInfo,
        ( device, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetDeviceInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATECONTEXT )
cl_context
SCOREP_LIBWRAP_FUNC_NAME( clCreateContext )( const cl_context_properties* properties,
                                             cl_uint                      numDevices,
                                             const cl_device_id*          devices,
                                             void( CL_CALLBACK * pfnNotify )( const char*,
                                                                              const void*,
                                                                              size_t,
                                                                              void* ),
                                             void*                        userData,
                                             cl_int*                      errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateContext );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_context ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateContext,
        ( properties, numDevices, devices, pfnNotify, userData, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateContext );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATECONTEXTFROMTYPE )
cl_context
SCOREP_LIBWRAP_FUNC_NAME( clCreateContextFromType )( const cl_context_properties* properties,
                                                     cl_device_type               deviceType,
                                                     void( CL_CALLBACK * pfnNotify )( const char*,
                                                                                      const void*,
                                                                                      size_t,
                                                                                      void* ),
                                                     void*                        userData,
                                                     cl_int*                      errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateContextFromType );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_context ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateContextFromType,
        ( properties, deviceType, pfnNotify, userData, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateContextFromType );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINCONTEXT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainContext )( cl_context context )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainContext );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainContext, ( context ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainContext );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASECONTEXT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseContext )( cl_context context )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseContext );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseContext, ( context ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseContext );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETCONTEXTINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetContextInfo )( cl_context      context,
                                              cl_context_info paramName,
                                              size_t          paramValueSize,
                                              void*           paramValue,
                                              size_t*         paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetContextInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetContextInfo,
        ( context, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetContextInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINCOMMANDQUEUE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainCommandQueue )( cl_command_queue commandQueue )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainCommandQueue );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clRetainCommandQueue, ( commandQueue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainCommandQueue );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASECOMMANDQUEUE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseCommandQueue )( cl_command_queue commandQueue )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseCommandQueue );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clReleaseCommandQueue, ( commandQueue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseCommandQueue );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETCOMMANDQUEUEINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetCommandQueueInfo )( cl_command_queue      commandQueue,
                                                   cl_command_queue_info paramName,
                                                   size_t                paramValueSize,
                                                   void*                 paramValue,
                                                   size_t*               paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetCommandQueueInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetCommandQueueInfo,
        ( commandQueue, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetCommandQueueInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLSETCOMMANDQUEUEPROPERTY )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetCommandQueueProperty )( cl_command_queue             commandQueue,
                                                       cl_command_queue_properties  properties,
                                                       cl_bool                      enable,
                                                       cl_command_queue_properties* oldProperties )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetCommandQueueProperty );

    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy ) )
    {
        properties |= CL_QUEUE_PROFILING_ENABLE;
    }

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetCommandQueueProperty,
        ( commandQueue, properties, enable, oldProperties ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetCommandQueueProperty );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEBUFFER )
cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateBuffer )( cl_context   context,
                                            cl_mem_flags flags,
                                            size_t       size,
                                            void*        hostPtr,
                                            cl_int*      errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateBuffer,
        ( context, flags, size, hostPtr, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateBuffer );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINMEMOBJECT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainMemObject )( cl_mem memObj )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainMemObject );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainMemObject, ( memObj ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainMemObject );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEMEMOBJECT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseMemObject )( cl_mem memObj )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseMemObject );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseMemObject, ( memObj ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseMemObject );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETSUPPORTEDIMAGEFORMATS )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetSupportedImageFormats )( cl_context         context,
                                                        cl_mem_flags       flags,
                                                        cl_mem_object_type imageType,
                                                        cl_uint            numEntries,
                                                        cl_image_format*   imageFormats,
                                                        cl_uint*           numImageFormats )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetSupportedImageFormats );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetSupportedImageFormats,
        ( context, flags, imageType, numEntries, imageFormats, numImageFormats ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetSupportedImageFormats );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETMEMOBJECTINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetMemObjectInfo )( cl_mem      memObj,
                                                cl_mem_info paramName,
                                                size_t      paramValueSize,
                                                void*       paramValue,
                                                size_t*     paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetMemObjectInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetMemObjectInfo,
        ( memObj, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetMemObjectInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETIMAGEINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetImageInfo )( cl_mem        image,
                                            cl_image_info paramName,
                                            size_t        paramValueSize,
                                            void*         paramValue,
                                            size_t*       paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetImageInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetImageInfo,
        ( image, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetImageInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINSAMPLER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainSampler )( cl_sampler sampler )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainSampler );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainSampler, ( sampler ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainSampler );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASESAMPLER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseSampler )( cl_sampler sampler )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseSampler );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseSampler, ( sampler ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseSampler );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETSAMPLERINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetSamplerInfo )( cl_sampler      sampler,
                                              cl_sampler_info paramName,
                                              size_t          paramValueSize,
                                              void*           paramValue,
                                              size_t*         paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetSamplerInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetSamplerInfo,
        ( sampler, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetSamplerInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEPROGRAMWITHSOURCE )
cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithSource )( cl_context    context,
                                                       cl_uint       count,
                                                       const char**  strings,
                                                       const size_t* lengths,
                                                       cl_int*       errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateProgramWithSource );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_program ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateProgramWithSource,
        ( context, count, strings, lengths, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateProgramWithSource );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEPROGRAMWITHBINARY )
cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithBinary )( cl_context            context,
                                                       cl_uint               numDevices,
                                                       const cl_device_id*   deviceList,
                                                       const size_t*         lengths,
                                                       const unsigned char** binaries,
                                                       cl_int*               binaryStatus,
                                                       cl_int*               errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateProgramWithBinary );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_program ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateProgramWithBinary,
        ( context, numDevices, deviceList, lengths, binaries, binaryStatus, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateProgramWithBinary );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINPROGRAM )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainProgram )( cl_program program )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainProgram, ( program ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainProgram );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEPROGRAM )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseProgram )( cl_program program )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseProgram, ( program ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseProgram );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLBUILDPROGRAM )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clBuildProgram )( cl_program          program,
                                            cl_uint             numDevices,
                                            const cl_device_id* deviceList,
                                            const char*         options,
                                            void( CL_CALLBACK * pfnNotify )( cl_program,
                                                                             void* ),
                                            void*               user_data )
{
    SCOREP_OPENCL_FUNC_ENTER( clBuildProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clBuildProgram,
        ( program, numDevices, deviceList, options, pfnNotify, user_data ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clBuildProgram );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETPROGRAMINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetProgramInfo )( cl_program      program,
                                              cl_program_info paramName,
                                              size_t          paramValueSize,
                                              void*           paramValue,
                                              size_t*         paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetProgramInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetProgramInfo,
        ( program, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetProgramInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETPROGRAMBUILDINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetProgramBuildInfo )( cl_program            program,
                                                   cl_device_id          device,
                                                   cl_program_build_info paramName,
                                                   size_t                paramValueSize,
                                                   void*                 paramValue,
                                                   size_t*               paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetProgramBuildInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetProgramBuildInfo,
        ( program, device, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetProgramBuildInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEKERNEL )
cl_kernel
SCOREP_LIBWRAP_FUNC_NAME( clCreateKernel )( cl_program  program,
                                            const char* kernelName,
                                            cl_int*     errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateKernel );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_kernel ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateKernel, ( program, kernelName, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateKernel );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEKERNELSINPROGRAM )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCreateKernelsInProgram )( cl_program program,
                                                      cl_uint    numKernels,
                                                      cl_kernel* kernels,
                                                      cl_uint*   numKernelsRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateKernelsInProgram );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateKernelsInProgram,
        ( program, numKernels, kernels, numKernelsRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateKernelsInProgram );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINKERNEL )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainKernel )( cl_kernel kernel )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainKernel );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainKernel, ( kernel ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainKernel );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEKERNEL )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseKernel )( cl_kernel kernel )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseKernel );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseKernel, ( kernel ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseKernel );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLSETKERNELARG )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelArg )( cl_kernel   kernel,
                                            cl_uint     argIndex,
                                            size_t      argSize,
                                            const void* argValue )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetKernelArg );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetKernelArg, ( kernel, argIndex, argSize, argValue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetKernelArg );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETKERNELINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelInfo )( cl_kernel      kernel,
                                             cl_kernel_info paramName,
                                             size_t         paramValueSize,
                                             void*          paramValue,
                                             size_t*        paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetKernelInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetKernelInfo,
        ( kernel, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetKernelInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETKERNELWORKGROUPINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelWorkGroupInfo )( cl_kernel                 kernel,
                                                      cl_device_id              device,
                                                      cl_kernel_work_group_info paramName,
                                                      size_t                    paramValueSize,
                                                      void*                     paramValue,
                                                      size_t*                   paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetKernelWorkGroupInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetKernelWorkGroupInfo,
        ( kernel, device, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetKernelWorkGroupInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLWAITFOREVENTS )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clWaitForEvents )( cl_uint         numEvents,
                                             const cl_event* eventList )
{
    SCOREP_OPENCL_FUNC_ENTER( clWaitForEvents );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clWaitForEvents, ( numEvents, eventList ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clWaitForEvents );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETEVENTINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetEventInfo )( cl_event      event,
                                            cl_event_info paramName,
                                            size_t        paramValueSize,
                                            void*         paramValue,
                                            size_t*       paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetEventInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetEventInfo,
        ( event, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetEventInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINEVENT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainEvent )( cl_event event )
{
    SCOREP_OPENCL_FUNC_ENTER( clRetainEvent );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clRetainEvent, ( event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clRetainEvent );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEEVENT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseEvent )( cl_event event )
{
    SCOREP_OPENCL_FUNC_ENTER( clReleaseEvent );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clReleaseEvent, ( event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clReleaseEvent );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLGETEVENTPROFILINGINFO )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetEventProfilingInfo )( cl_event          event,
                                                     cl_profiling_info paramName,
                                                     size_t            paramValueSize,
                                                     void*             paramValue,
                                                     size_t*           paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetEventProfilingInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetEventProfilingInfo,
        ( event, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetEventProfilingInfo );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLFLUSH )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clFlush )( cl_command_queue commandQueue )
{
    SCOREP_OPENCL_FUNC_ENTER( clFlush );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clFlush, ( commandQueue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clFlush );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLFINISH )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clFinish )( cl_command_queue commandQueue )
{
    SCOREP_OPENCL_FUNC_ENTER( clFinish );

    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy ) )
    {
        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENCL, "[OpenCL] Finish command queue %p", commandQueue );
        scorep_opencl_queue* queue = scorep_opencl_queue_get( commandQueue );

        UTILS_MutexLock( &queue->mutex );
        scorep_opencl_queue_flush( queue );
        UTILS_MutexUnlock( &queue->mutex );
    }

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clFinish, ( commandQueue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clFinish );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEREADBUFFER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadBuffer )( cl_command_queue commandQueue,
                                                 cl_mem           buffer,
                                                 cl_bool          blockingRead,
                                                 size_t           offset,
                                                 size_t           size,
                                                 void*            ptr,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    SCOREP_OPENCL_ENQUEUE_BUFFER( clEnqueueReadBuffer,
                                  SCOREP_ENQUEUE_BUFFER_DEV2HOST,
                                  size, commandQueue, event, blockingRead,
                                  ( commandQueue, buffer, blockingRead,
                                    offset, size, ptr, numEventsInWaitList,
                                    eventWaitList, event )
                                  )
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWRITEBUFFER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteBuffer )( cl_command_queue commandQueue,
                                                  cl_mem           buffer,
                                                  cl_bool          blockingWrite,
                                                  size_t           offset,
                                                  size_t           size,
                                                  const void*      ptr,
                                                  cl_uint          numEventsInWaitList,
                                                  const cl_event*  eventWaitList,
                                                  cl_event*        event )
{
    SCOREP_OPENCL_ENQUEUE_BUFFER( clEnqueueWriteBuffer,
                                  SCOREP_ENQUEUE_BUFFER_HOST2DEV,
                                  size, commandQueue, event, blockingWrite,
                                  ( commandQueue, buffer, blockingWrite,
                                    offset, size, ptr, numEventsInWaitList,
                                    eventWaitList, event )
                                  )
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYBUFFER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBuffer )( cl_command_queue commandQueue,
                                                 cl_mem           srcBuffer,
                                                 cl_mem           dstBuffer,
                                                 size_t           srcOffset,
                                                 size_t           dstOffset,
                                                 size_t           size,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueCopyBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueCopyBuffer,
        ( commandQueue, srcBuffer, dstBuffer, srcOffset, dstOffset, size,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueCopyBuffer );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEREADIMAGE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadImage )( cl_command_queue commandQueue,
                                                cl_mem           image,
                                                cl_bool          blockingRead,
                                                const size_t*    origin,
                                                const size_t*    region,
                                                size_t           rowPitch,
                                                size_t           slicePitch,
                                                void*            ptr,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueReadImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueReadImage,
        ( commandQueue, image, blockingRead, origin, region, rowPitch,
          slicePitch, ptr, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueReadImage );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWRITEIMAGE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteImage )( cl_command_queue commandQueue,
                                                 cl_mem           image,
                                                 cl_bool          blockingWrite,
                                                 const size_t*    origin,
                                                 const size_t*    region,
                                                 size_t           inputRowPitch,
                                                 size_t           inputSlicePitch,
                                                 const void*      ptr,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueWriteImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueWriteImage,
        ( commandQueue, image, blockingWrite, origin, region, inputRowPitch,
          inputSlicePitch, ptr, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueWriteImage );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYIMAGE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyImage )( cl_command_queue commandQueue,
                                                cl_mem           srcImage,
                                                cl_mem           dstImage,
                                                const size_t*    srcOrigin,
                                                const size_t*    dstOrigin,
                                                const size_t*    region,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueCopyImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueCopyImage,
        ( commandQueue, srcImage, dstImage, srcOrigin, dstOrigin, region,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueCopyImage );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYIMAGETOBUFFER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyImageToBuffer )( cl_command_queue commandQueue,
                                                        cl_mem           srcImage,
                                                        cl_mem           dstBuffer,
                                                        const size_t*    srcOrigin,
                                                        const size_t*    region,
                                                        size_t           dstOffset,
                                                        cl_uint          numEventsInWaitList,
                                                        const cl_event*  eventWaitList,
                                                        cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueCopyImageToBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueCopyImageToBuffer,
        ( commandQueue, srcImage, dstBuffer, srcOrigin, region, dstOffset,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueCopyImageToBuffer );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYBUFFERTOIMAGE )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBufferToImage )( cl_command_queue commandQueue,
                                                        cl_mem           srcBuffer,
                                                        cl_mem           dstImage,
                                                        size_t           srcOffset,
                                                        const size_t*    dstOrigin,
                                                        const size_t*    region,
                                                        cl_uint          numEventsInWaitList,
                                                        const cl_event*  eventWaitList,
                                                        cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueCopyBufferToImage );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueCopyBufferToImage,
        ( commandQueue, srcBuffer, dstImage, srcOffset, dstOrigin, region,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueCopyBufferToImage );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMAPBUFFER )
void*
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMapBuffer )( cl_command_queue commandQueue,
                                                cl_mem           buffer,
                                                cl_bool          blockingMap,
                                                cl_map_flags     mapFlags,
                                                size_t           offset,
                                                size_t           size,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event,
                                                cl_int*          errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueMapBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    void* ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueMapBuffer,
        ( commandQueue, buffer, blockingMap, mapFlags, offset, size,
          numEventsInWaitList, eventWaitList, event, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueMapBuffer );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMAPIMAGE )
void*
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMapImage )( cl_command_queue commandQueue,
                                               cl_mem           image,
                                               cl_bool          blockingMap,
                                               cl_map_flags     mapFlags,
                                               const size_t*    origin,
                                               const size_t*    region,
                                               size_t*          imageRowPitch,
                                               size_t*          imageSlicePitch,
                                               cl_uint          numEventsInWaitList,
                                               const cl_event*  eventWaitList,
                                               cl_event*        event,
                                               cl_int*          errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueMapImage );

    SCOREP_OPENCL_WRAP_ENTER();
    void* ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueMapImage,
        ( commandQueue, image, blockingMap, mapFlags, origin, region,
          imageRowPitch, imageSlicePitch, numEventsInWaitList, eventWaitList,
          event, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueMapImage );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEUNMAPMEMOBJECT )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueUnmapMemObject )( cl_command_queue commandQueue,
                                                     cl_mem           memObj,
                                                     void*            mappedPtr,
                                                     cl_uint          numEventsInWaitList,
                                                     const cl_event*  eventWaitList,
                                                     cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueUnmapMemObject );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueUnmapMemObject,
        ( commandQueue, memObj, mappedPtr, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueUnmapMemObject );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUENDRANGEKERNEL )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueNDRangeKernel )( cl_command_queue commandQueue,
                                                    cl_kernel        clKernel,
                                                    cl_uint          workDim,
                                                    const size_t*    globalWorkOffset,
                                                    const size_t*    globalWorkSize,
                                                    const size_t*    localWorkSize,
                                                    cl_uint          numEventsInWaitList,
                                                    const cl_event*  eventWaitList,
                                                    cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueNDRangeKernel );

    scorep_opencl_buffer_entry* kernel = NULL;
    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && scorep_opencl_record_kernels )
    {
        kernel = scorep_opencl_get_buffer_entry( scorep_opencl_queue_get( commandQueue ) );

        if ( kernel && event == NULL )
        {
            event = &( kernel->event );
        }
    }

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueNDRangeKernel,
        ( commandQueue, clKernel, workDim, globalWorkOffset, globalWorkSize,
          localWorkSize, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && scorep_opencl_record_kernels )
    {
        if ( kernel && CL_SUCCESS == ret )
        {
            kernel->u.kernel = clKernel;
            scorep_opencl_retain_kernel( kernel );
        }
    }

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueNDRangeKernel );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUENATIVEKERNEL )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueNativeKernel )( cl_command_queue commandQueue,
                                                   void( CL_CALLBACK * pfnNotify )( void* ),
                                                   void*            args,
                                                   size_t           cbArgs,
                                                   cl_uint          numMemObjects,
                                                   const cl_mem*    memList,
                                                   const void**     argsMemLoc,
                                                   cl_uint          numEventsInWaitList,
                                                   const cl_event*  eventWaitList,
                                                   cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueNativeKernel );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueNativeKernel,
        ( commandQueue, pfnNotify, args, cbArgs, numMemObjects, memList,
          argsMemLoc, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueNativeKernel );

    return ret;
}



/*
 *
 * Deprecated OpenCL APIs
 *
 * Marked as deprecated since OpenCL 1.1
 *
 */

cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage2D )( cl_context             context,
                                             cl_mem_flags           flags,
                                             const cl_image_format* imageFormat,
                                             size_t                 imageWidth,
                                             size_t                 imageHeight,
                                             size_t                 imageRowPitch,
                                             void*                  hostPtr,
                                             cl_int*                errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateImage2D );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateImage2D,
        ( context, flags, imageFormat, imageWidth, imageHeight,
          imageRowPitch, hostPtr, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateImage2D );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEIMAGE3D )
cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage3D )( cl_context             context,
                                             cl_mem_flags           flags,
                                             const cl_image_format* imageFormat,
                                             size_t                 imageWidth,
                                             size_t                 imageHeight,
                                             size_t                 imageDepth,
                                             size_t                 imageRowPitch,
                                             size_t                 imageSlicePitch,
                                             void*                  hostPtr,
                                             cl_int*                errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateImage3D );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateImage3D,
        ( context, flags, imageFormat, imageWidth, imageHeight, imageDepth,
          imageRowPitch, imageSlicePitch, hostPtr, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateImage3D );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMARKER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMarker )( cl_command_queue commandQueue,
                                             cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueMarker );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clEnqueueMarker, ( commandQueue, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueMarker );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWAITFOREVENTS )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWaitForEvents )( cl_command_queue commandQueue,
                                                    cl_uint          numEvents,
                                                    const cl_event*  eventList )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueWaitForEvents );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueWaitForEvents,
        ( commandQueue, numEvents, eventList ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueWaitForEvents );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEBARRIER )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueBarrier )( cl_command_queue commandQueue )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueBarrier );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clEnqueueBarrier, ( commandQueue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueBarrier );

    return ret;
}

cl_int
SCOREP_LIBWRAP_FUNC_NAME( clUnloadCompiler )( void )
{
    SCOREP_OPENCL_FUNC_ENTER( clUnloadCompiler );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL( clUnloadCompiler, ( /* void */ ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clUnloadCompiler );

    return ret;
}

void*
SCOREP_LIBWRAP_FUNC_NAME( clGetExtensionFunctionAddress )( const char* funcName )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetExtensionFunctionAddress );

    SCOREP_OPENCL_WRAP_ENTER();
    void* ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetExtensionFunctionAddress, ( funcName ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetExtensionFunctionAddress );

    return ret;
}



/*
 *
 * Deprecated OpenCL APIs
 *
 * Marked as deprecated since OpenCL 2.0
 *
 */

cl_command_queue
SCOREP_LIBWRAP_FUNC_NAME( clCreateCommandQueue )( cl_context                  context,
                                                  cl_device_id                device,
                                                  cl_command_queue_properties properties,
                                                  cl_int*                     errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateCommandQueue );

    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy ) )
    {
        properties |= CL_QUEUE_PROFILING_ENABLE;
    }

    SCOREP_OPENCL_WRAP_ENTER();
    cl_command_queue ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateCommandQueue, ( context, device, properties, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    if ( trigger
         && SCOREP_IS_MEASUREMENT_PHASE( WITHIN )
         && ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy ) )
    {
        scorep_opencl_queue* queue = scorep_opencl_queue_create( ret, device );
    }

    SCOREP_OPENCL_FUNC_EXIT( clCreateCommandQueue );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLCREATESAMPLER )
cl_sampler
SCOREP_LIBWRAP_FUNC_NAME( clCreateSampler )( cl_context         context,
                                             cl_bool            normalizedCoords,
                                             cl_addressing_mode addressingMode,
                                             cl_filter_mode     filterMode,
                                             cl_int*            errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateSampler );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_sampler ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateSampler,
        ( context, normalizedCoords, addressingMode, filterMode, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateSampler );

    return ret;
}
#endif



#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUETASK )
cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueTask )( cl_command_queue commandQueue,
                                           cl_kernel        kernel,
                                           cl_uint          numEventsInWaitList,
                                           const cl_event*  eventWaitList,
                                           cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueTask );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueTask,
        ( commandQueue, kernel, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueTask );

    return ret;
}
#endif
