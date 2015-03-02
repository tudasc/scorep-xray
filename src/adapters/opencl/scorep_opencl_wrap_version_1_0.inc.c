/* *INDENT-OFF* */

cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPlatformIDs ) ( cl_uint         numEntries,
                                               cl_platform_id* platforms,
                                               cl_uint*        numPlatforms )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetPlatformIDs );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetPlatformIDs,
                                             clGetPlatformIDs,
                                             ( numEntries, platforms,
                                               numPlatforms ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetPlatformIDs );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPlatformInfo ) ( cl_platform_id   platform,
                                                cl_platform_info paramName,
                                                size_t           paramValueSize,
                                                void*            paramValue,
                                                size_t*          paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetPlatformInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetPlatformInfo,
                                             clGetPlatformInfo,
                                             ( platform, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetPlatformInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetDeviceIDs ) ( cl_platform_id platform,
                                             cl_device_type deviceType,
                                             cl_uint        numEntries,
                                             cl_device_id*  devices,
                                             cl_uint*       numDevices )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetDeviceIDs );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetDeviceIDs,
                                             clGetDeviceIDs,
                                             ( platform, deviceType, numEntries,
                                               devices, numDevices ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetDeviceIDs );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetDeviceInfo ) ( cl_device_id   device,
                                              cl_device_info paramName,
                                              size_t         paramValueSize,
                                              void*          paramValue,
                                              size_t*        paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetDeviceInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetDeviceInfo,
                                             clGetDeviceInfo,
                                             ( device, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetDeviceInfo );

    return ret;
}



cl_context
SCOREP_LIBWRAP_FUNC_NAME( clCreateContext ) ( const cl_context_properties* properties,
                                              cl_uint                      numDevices,
                                              const cl_device_id*          devices,
                                              void ( CL_CALLBACK* pfnNotify )( const char*,
                                                                               const void*,
                                                                               size_t,
                                                                               void* ),
                                              void*                        userData,
                                              cl_int*                      errcodeRet )
{
    cl_context ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateContext );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateContext,
                                             clCreateContext,
                                             ( properties, numDevices, devices,
                                               pfnNotify, userData, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateContext );

    return ret;
}



cl_context
SCOREP_LIBWRAP_FUNC_NAME( clCreateContextFromType ) ( const cl_context_properties* properties,
                                                      cl_device_type               deviceType,
                                                      void ( CL_CALLBACK* pfnNotify )( const char*,
                                                                                       const void*,
                                                                                       size_t,
                                                                                       void* ),
                                                      void*                        userData,
                                                      cl_int*                      errcodeRet )
{
    cl_context ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateContextFromType );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateContextFromType,
                                             clCreateContextFromType,
                                             ( properties, deviceType, pfnNotify,
                                               userData, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateContextFromType );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainContext ) ( cl_context context )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainContext );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainContext,
                                             clRetainContext,
                                             ( context ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainContext );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseContext ) ( cl_context context )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseContext );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseContext, clReleaseContext,
                                             ( context ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseContext );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetContextInfo ) ( cl_context      context,
                                               cl_context_info paramName,
                                               size_t          paramValueSize,
                                               void*           paramValue,
                                               size_t*         paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetContextInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetContextInfo, clGetContextInfo,
                                             ( context, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetContextInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainCommandQueue ) ( cl_command_queue commandQueue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainCommandQueue );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainCommandQueue, clRetainCommandQueue,
                                             ( commandQueue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainCommandQueue );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseCommandQueue ) ( cl_command_queue commandQueue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseCommandQueue );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseCommandQueue, clReleaseCommandQueue,
                                             ( commandQueue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseCommandQueue );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetCommandQueueInfo ) ( cl_command_queue      commandQueue,
                                                    cl_command_queue_info paramName,
                                                    size_t                paramValueSize,
                                                    void*                 paramValue,
                                                    size_t*               paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetCommandQueueInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetCommandQueueInfo, clGetCommandQueueInfo,
                                             ( commandQueue, paramName,
                                               paramValueSize, paramValue,
                                               paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetCommandQueueInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetCommandQueueProperty ) ( cl_command_queue             commandQueue,
                                                        cl_command_queue_properties  properties,
                                                        cl_bool                      enable,
                                                        cl_command_queue_properties* oldProperties )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetCommandQueueProperty );

    if ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy )
    {
        properties |= CL_QUEUE_PROFILING_ENABLE;
    }

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetCommandQueueProperty, clSetCommandQueueProperty,
                                             ( commandQueue, properties,
                                               enable, oldProperties ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetCommandQueueProperty );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateBuffer ) ( cl_context   context,
                                             cl_mem_flags flags,
                                             size_t       size,
                                             void*        hostPtr,
                                             cl_int*      errcodeRet )
{
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateBuffer, clCreateBuffer,
                                             ( context, flags, size, hostPtr,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainMemObject ) ( cl_mem memObj )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainMemObject );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainMemObject, clRetainMemObject,
                                             ( memObj ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainMemObject );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseMemObject ) ( cl_mem memObj )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseMemObject );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseMemObject, clReleaseMemObject,
                                             ( memObj ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseMemObject );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetSupportedImageFormats ) ( cl_context         context,
                                                         cl_mem_flags       flags,
                                                         cl_mem_object_type imageType,
                                                         cl_uint            numEntries,
                                                         cl_image_format*   imageFormats,
                                                         cl_uint*           numImageFormats )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetSupportedImageFormats );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetSupportedImageFormats, clGetSupportedImageFormats,
                                             ( context, flags, imageType,
                                               numEntries, imageFormats,
                                               numImageFormats ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetSupportedImageFormats );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetMemObjectInfo ) ( cl_mem      memObj,
                                                 cl_mem_info paramName,
                                                 size_t      paramValueSize,
                                                 void*       paramValue,
                                                 size_t*     paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetMemObjectInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetMemObjectInfo, clGetMemObjectInfo,
                                             ( memObj, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetMemObjectInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetImageInfo ) ( cl_mem        image,
                                             cl_image_info paramName,
                                             size_t        paramValueSize,
                                             void*         paramValue,
                                             size_t*       paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetImageInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetImageInfo, clGetImageInfo,
                                             ( image, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetImageInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainSampler ) ( cl_sampler sampler )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainSampler );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainSampler, clRetainSampler,
                                             ( sampler ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainSampler );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseSampler ) ( cl_sampler sampler )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseSampler );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseSampler, clReleaseSampler,
                                             ( sampler ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseSampler );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetSamplerInfo ) ( cl_sampler      sampler,
                                               cl_sampler_info paramName,
                                               size_t          paramValueSize,
                                               void*           paramValue,
                                               size_t*         paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetSamplerInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetSamplerInfo, clGetSamplerInfo,
                                             ( sampler, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetSamplerInfo );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithSource ) ( cl_context    context,
                                                        cl_uint       count,
                                                        const char**  strings,
                                                        const size_t* lengths,
                                                        cl_int*       errcodeRet )
{
    cl_program ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateProgramWithSource );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateProgramWithSource, clCreateProgramWithSource,
                                             ( context, count, strings, lengths,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateProgramWithSource );

    return ret;
}



cl_program
SCOREP_LIBWRAP_FUNC_NAME( clCreateProgramWithBinary ) ( cl_context            context,
                                                        cl_uint               numDevices,
                                                        const cl_device_id*   deviceList,
                                                        const size_t*         lengths,
                                                        const unsigned char** binaries,
                                                        cl_int*               binaryStatus,
                                                        cl_int*               errcodeRet )
{
    cl_program ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateProgramWithBinary );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateProgramWithBinary, clCreateProgramWithBinary,
                                             ( context, numDevices, deviceList,
                                               lengths, binaries, binaryStatus,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateProgramWithBinary );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainProgram ) ( cl_program program )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainProgram, clRetainProgram,
                                             ( program ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseProgram ) ( cl_program program )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseProgram, clReleaseProgram,
                                             ( program ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clBuildProgram ) ( cl_program          program,
                                             cl_uint             numDevices,
                                             const cl_device_id* deviceList,
                                             const char*         options,
                                             void ( CL_CALLBACK* pfnNotify )( cl_program,
                                                                              void* ),
                                             void*               user_data )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clBuildProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clBuildProgram, clBuildProgram,
                                             ( program, numDevices, deviceList,
                                               options, pfnNotify, user_data ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clBuildProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetProgramInfo ) ( cl_program      program,
                                               cl_program_info paramName,
                                               size_t          paramValueSize,
                                               void*           paramValue,
                                               size_t*         paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetProgramInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetProgramInfo, clGetProgramInfo,
                                             ( program, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetProgramInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetProgramBuildInfo ) ( cl_program            program,
                                                    cl_device_id          device,
                                                    cl_program_build_info paramName,
                                                    size_t                paramValueSize,
                                                    void*                 paramValue,
                                                    size_t*               paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetProgramBuildInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetProgramBuildInfo, clGetProgramBuildInfo,
                                             ( program, device, paramName,
                                               paramValueSize, paramValue,
                                               paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetProgramBuildInfo );

    return ret;
}



cl_kernel
SCOREP_LIBWRAP_FUNC_NAME( clCreateKernel ) ( cl_program  program,
                                             const char* kernelName,
                                             cl_int*     errcodeRet )
{
    cl_kernel ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateKernel );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateKernel, clCreateKernel,
                                             ( program, kernelName, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateKernel );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clCreateKernelsInProgram ) ( cl_program program,
                                                       cl_uint    numKernels,
                                                       cl_kernel* kernels,
                                                       cl_uint*   numKernelsRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateKernelsInProgram );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateKernelsInProgram, clCreateKernelsInProgram,
                                             ( program, numKernels,
                                               kernels, numKernelsRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateKernelsInProgram );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainKernel ) ( cl_kernel kernel )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainKernel );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainKernel, clRetainKernel,
                                             ( kernel ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainKernel );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseKernel ) ( cl_kernel kernel )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseKernel );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseKernel, clReleaseKernel,
                                             ( kernel ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseKernel );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelArg ) ( cl_kernel   kernel,
                                             cl_uint     argIndex,
                                             size_t      argSize,
                                             const void* argValue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetKernelArg );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetKernelArg, clSetKernelArg,
                                             ( kernel, argIndex, argSize, argValue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetKernelArg );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelInfo ) ( cl_kernel      kernel,
                                              cl_kernel_info paramName,
                                              size_t         paramValueSize,
                                              void*          paramValue,
                                              size_t*        paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetKernelInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetKernelInfo, clGetKernelInfo,
                                             ( kernel, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetKernelInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetKernelWorkGroupInfo ) ( cl_kernel                 kernel,
                                                       cl_device_id              device,
                                                       cl_kernel_work_group_info paramName,
                                                       size_t                    paramValueSize,
                                                       void*                     paramValue,
                                                       size_t*                   paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetKernelWorkGroupInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetKernelWorkGroupInfo, clGetKernelWorkGroupInfo,
                                             ( kernel, device, paramName,
                                               paramValueSize, paramValue,
                                               paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetKernelWorkGroupInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clWaitForEvents ) ( cl_uint         numEvents,
                                              const cl_event* eventList )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clWaitForEvents );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clWaitForEvents, clWaitForEvents,
                                             ( numEvents, eventList ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clWaitForEvents );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetEventInfo ) ( cl_event      event,
                                             cl_event_info paramName,
                                             size_t        paramValueSize,
                                             void*         paramValue,
                                             size_t*       paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetEventInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetEventInfo, clGetEventInfo,
                                             ( event, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetEventInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clRetainEvent ) ( cl_event event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clRetainEvent );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clRetainEvent, clRetainEvent,
                                             ( event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clRetainEvent );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clReleaseEvent ) ( cl_event event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clReleaseEvent );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clReleaseEvent, clReleaseEvent,
                                             ( event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clReleaseEvent );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetEventProfilingInfo ) ( cl_event          event,
                                                      cl_profiling_info paramName,
                                                      size_t            paramValueSize,
                                                      void*             paramValue,
                                                      size_t*           paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetEventProfilingInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetEventProfilingInfo, clGetEventProfilingInfo,
                                             ( event, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetEventProfilingInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clFlush ) ( cl_command_queue commandQueue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clFlush );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clFlush, clFlush,
                                             ( commandQueue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clFlush );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clFinish ) ( cl_command_queue commandQueue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clFinish );

    if ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy )
    {
        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENCL, "[OpenCL] Finish command queue %p", commandQueue );
        scorep_opencl_queue *queue = scorep_opencl_queue_get( commandQueue );

        SCOREP_MutexLock( queue->mutex );
        scorep_opencl_queue_flush( queue );
        SCOREP_MutexUnlock( queue->mutex );
    }

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clFinish, clFinish,
                                             ( commandQueue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clFinish );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadBuffer ) ( cl_command_queue commandQueue,
                                                  cl_mem           buffer,
                                                  cl_bool          blockingRead,
                                                  size_t           offset,
                                                  size_t           size,
                                                  void*            ptr,
                                                  cl_uint          numEventsInWaitList,
                                                  const cl_event*  eventWaitList,
                                                  cl_event*        event )
{
    SCOREP_OPENCL_ENQUEUE_BUFFER( scorep_opencl_region__clEnqueueReadBuffer,
                                  SCOREP_ENQUEUE_BUFFER_DEV2HOST,
                                  size, commandQueue, event, blockingRead,
                                  SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueReadBuffer, clEnqueueReadBuffer,
                                                                           ( commandQueue, buffer, blockingRead,
                                                                             offset, size, ptr, numEventsInWaitList,
                                                                             eventWaitList, event ) )
                                  )
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteBuffer ) ( cl_command_queue commandQueue,
                                                   cl_mem           buffer,
                                                   cl_bool          blockingWrite,
                                                   size_t           offset,
                                                   size_t           size,
                                                   const void*      ptr,
                                                   cl_uint          numEventsInWaitList,
                                                   const cl_event*  eventWaitList,
                                                   cl_event*        event )
{
    SCOREP_OPENCL_ENQUEUE_BUFFER( scorep_opencl_region__clEnqueueWriteBuffer,
                                  SCOREP_ENQUEUE_BUFFER_HOST2DEV,
                                  size, commandQueue, event, blockingWrite,
                                  SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueWriteBuffer, clEnqueueWriteBuffer,
                                                                           ( commandQueue, buffer, blockingWrite,
                                                                             offset, size, ptr, numEventsInWaitList,
                                                                             eventWaitList, event ) );
                                  )
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBuffer ) ( cl_command_queue commandQueue,
                                                  cl_mem           srcBuffer,
                                                  cl_mem           dstBuffer,
                                                  size_t           srcOffset,
                                                  size_t           dstOffset,
                                                  size_t           size,
                                                  cl_uint          numEventsInWaitList,
                                                  const cl_event*  eventWaitList,
                                                  cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueCopyBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueCopyBuffer, clEnqueueCopyBuffer,
                                             ( commandQueue, srcBuffer, dstBuffer,
                                               srcOffset, dstOffset, size,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueCopyBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadImage ) ( cl_command_queue commandQueue,
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
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueReadImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueReadImage,
                                             clEnqueueReadImage,
                                             ( commandQueue, image, blockingRead,
                                               origin, region, rowPitch,
                                               slicePitch, ptr, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueReadImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteImage ) ( cl_command_queue commandQueue,
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
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueWriteImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueWriteImage, clEnqueueWriteImage,
                                             ( commandQueue, image, blockingWrite,
                                               origin, region, inputRowPitch,
                                               inputSlicePitch, ptr,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueWriteImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyImage ) ( cl_command_queue commandQueue,
                                                 cl_mem           srcImage,
                                                 cl_mem           dstImage,
                                                 const size_t*    srcOrigin,
                                                 const size_t*    dstOrigin,
                                                 const size_t*    region,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueCopyImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueCopyImage,
                                             clEnqueueCopyImage,
                                             ( commandQueue, srcImage, dstImage,
                                               srcOrigin, dstOrigin,
                                               region, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueCopyImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyImageToBuffer ) ( cl_command_queue commandQueue,
                                                         cl_mem           srcImage,
                                                         cl_mem           dstBuffer,
                                                         const size_t*    srcOrigin,
                                                         const size_t*    region,
                                                         size_t           dstOffset,
                                                         cl_uint          numEventsInWaitList,
                                                         const cl_event*  eventWaitList,
                                                         cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueCopyImageToBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueCopyImageToBuffer,
                                             clEnqueueCopyImageToBuffer,
                                             ( commandQueue, srcImage, dstBuffer,
                                               srcOrigin, region,
                                               dstOffset, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueCopyImageToBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBufferToImage ) ( cl_command_queue commandQueue,
                                                         cl_mem           srcBuffer,
                                                         cl_mem           dstImage,
                                                         size_t           srcOffset,
                                                         const size_t*    dstOrigin,
                                                         const size_t*    region,
                                                         cl_uint          numEventsInWaitList,
                                                         const cl_event*  eventWaitList,
                                                         cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueCopyBufferToImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueCopyBufferToImage,
                                             clEnqueueCopyBufferToImage,
                                             ( commandQueue, srcBuffer, dstImage,
                                               srcOffset, dstOrigin,
                                               region, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueCopyBufferToImage );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMapBuffer ) ( cl_command_queue commandQueue,
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
    void* ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueMapBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueMapBuffer, clEnqueueMapBuffer,
                                             ( commandQueue, buffer, blockingMap,
                                               mapFlags, offset, size,
                                               numEventsInWaitList, eventWaitList,
                                               event, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueMapBuffer );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMapImage ) ( cl_command_queue commandQueue,
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
    void* ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueMapImage );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueMapImage,
                                             clEnqueueMapImage,
                                             ( commandQueue, image, blockingMap,
                                               mapFlags, origin, region,
                                               imageRowPitch, imageSlicePitch,
                                               numEventsInWaitList, eventWaitList,
                                               event, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueMapImage );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueUnmapMemObject ) ( cl_command_queue commandQueue,
                                                      cl_mem           memObj,
                                                      void*            mappedPtr,
                                                      cl_uint          numEventsInWaitList,
                                                      const cl_event*  eventWaitList,
                                                      cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueUnmapMemObject );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueUnmapMemObject, clEnqueueUnmapMemObject,
                                             ( commandQueue, memObj, mappedPtr,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueUnmapMemObject );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueNDRangeKernel ) ( cl_command_queue commandQueue,
                                                     cl_kernel        clKernel,
                                                     cl_uint          workDim,
                                                     const size_t*    globalWorkOffset,
                                                     const size_t*    globalWorkSize,
                                                     const size_t*    localWorkSize,
                                                     cl_uint          numEventsInWaitList,
                                                     const cl_event*  eventWaitList,
                                                     cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueNDRangeKernel );

    if ( scorep_opencl_record_kernels )
    {
        scorep_opencl_buffer_entry* kernel =
            scorep_opencl_get_buffer_entry( scorep_opencl_queue_get( commandQueue ) );

        if ( kernel && event == NULL )
        {
            event = &( kernel->event );
        }

        ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueNDRangeKernel, clEnqueueNDRangeKernel,
                                                 ( commandQueue, clKernel, workDim,
                                                   globalWorkOffset, globalWorkSize,
                                                   localWorkSize, numEventsInWaitList,
                                                   eventWaitList, event ) );

        if ( kernel && CL_SUCCESS == ret )
        {
            kernel->u.kernel = clKernel;
            scorep_opencl_retain_kernel( kernel );
        }
    }
    else
    {
        ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueNDRangeKernel, clEnqueueNDRangeKernel,
                                                 ( commandQueue, clKernel, workDim,
                                                   globalWorkOffset, globalWorkSize,
                                                   localWorkSize, numEventsInWaitList,
                                                   eventWaitList, event ) );
    }

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueNDRangeKernel );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueNativeKernel ) ( cl_command_queue commandQueue,
                                                    void ( CL_CALLBACK* pfnNotify )( void* ),
                                                    void*            args,
                                                    size_t           cbArgs,
                                                    cl_uint          numMemObjects,
                                                    const cl_mem*    memList,
                                                    const void**     argsMemLoc,
                                                    cl_uint          numEventsInWaitList,
                                                    const cl_event*  eventWaitList,
                                                    cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueNativeKernel );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueNativeKernel, clEnqueueNativeKernel,
                                             ( commandQueue, pfnNotify, args,
                                               cbArgs, numMemObjects, memList,
                                               argsMemLoc, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueNativeKernel );

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
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage2D ) ( cl_context             context,
                                              cl_mem_flags           flags,
                                              const cl_image_format* imageFormat,
                                              size_t                 imageWidth,
                                              size_t                 imageHeight,
                                              size_t                 imageRowPitch,
                                              void*                  hostPtr,
                                              cl_int*                errcodeRet )
{
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateImage2D );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateImage2D, clCreateImage2D,
                                             ( context, flags, imageFormat,
                                               imageWidth, imageHeight,
                                               imageRowPitch, hostPtr,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateImage2D );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateImage3D ) ( cl_context             context,
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
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateImage3D );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateImage3D, clCreateImage3D,
                                             ( context, flags, imageFormat,
                                               imageWidth, imageHeight,
                                               imageDepth, imageRowPitch,
                                               imageSlicePitch, hostPtr,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateImage3D );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueMarker ) ( cl_command_queue commandQueue,
                                              cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueMarker );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueMarker, clEnqueueMarker,
                                             ( commandQueue, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueMarker );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWaitForEvents ) ( cl_command_queue commandQueue,
                                                     cl_uint          numEvents,
                                                     const cl_event*  eventList )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueWaitForEvents );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueWaitForEvents, clEnqueueWaitForEvents,
                                             ( commandQueue, numEvents,
                                               eventList ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueWaitForEvents );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueBarrier ) ( cl_command_queue commandQueue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueBarrier );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueBarrier, clEnqueueBarrier,
                                             ( commandQueue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueBarrier );

    return ret;
}

cl_int
SCOREP_LIBWRAP_FUNC_NAME( clUnloadCompiler ) ( void )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clUnloadCompiler );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clUnloadCompiler, clUnloadCompiler, ( /* void */ ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clUnloadCompiler );

    return ret;
}

void*
SCOREP_LIBWRAP_FUNC_NAME( clGetExtensionFunctionAddress ) ( const char* funcName )
{
    void* ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetExtensionFunctionAddress );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetExtensionFunctionAddress, clGetExtensionFunctionAddress,
                                             ( funcName ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetExtensionFunctionAddress );

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
SCOREP_LIBWRAP_FUNC_NAME( clCreateCommandQueue ) ( cl_context                  context,
                                                   cl_device_id                device,
                                                   cl_command_queue_properties properties,
                                                   cl_int*                     errcodeRet )
{
    cl_command_queue ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateCommandQueue );

    if ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy )
    {
        properties |= CL_QUEUE_PROFILING_ENABLE;
    }

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateCommandQueue, clCreateCommandQueue,
                                             ( context, device, properties,
                                               errcodeRet ) );

    if ( scorep_opencl_record_kernels || scorep_opencl_record_memcpy )
    {
        scorep_opencl_queue* queue = scorep_opencl_queue_create( ret, device );
    }

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateCommandQueue );

    return ret;
}



cl_sampler
SCOREP_LIBWRAP_FUNC_NAME( clCreateSampler ) ( cl_context         context,
                                              cl_bool            normalizedCoords,
                                              cl_addressing_mode addressingMode,
                                              cl_filter_mode     filterMode,
                                              cl_int*            errcodeRet )
{
    cl_sampler ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateSampler );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateSampler, clCreateSampler,
                                             ( context, normalizedCoords,
                                               addressingMode, filterMode,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateSampler );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueTask ) ( cl_command_queue commandQueue,
                                            cl_kernel        kernel,
                                            cl_uint          numEventsInWaitList,
                                            const cl_event*  eventWaitList,
                                            cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueTask );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueTask, clEnqueueTask,
                                             ( commandQueue, kernel,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueTask );

    return ret;
}

/* *INDENT-ON* */
