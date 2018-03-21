cl_command_queue
SCOREP_LIBWRAP_FUNC_NAME( clCreateCommandQueueWithProperties )( cl_context                 context,
                                                                cl_device_id               device,
                                                                const cl_queue_properties* properties,
                                                                cl_int*                    errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateCommandQueueWithProperties );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_command_queue ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateCommandQueueWithProperties,
        ( context, device, properties, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateCommandQueueWithProperties );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreatePipe )( cl_context                context,
                                          cl_mem_flags              flags,
                                          cl_uint                   pipePacketSize,
                                          cl_uint                   pipeMaxPackets,
                                          const cl_pipe_properties* properties,
                                          cl_int*                   errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreatePipe );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreatePipe,
        ( context, flags, pipePacketSize, pipeMaxPackets, properties, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreatePipe );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPipeInfo )( cl_mem       pipe,
                                           cl_pipe_info paramName,
                                           size_t       paramValueSize,
                                           void*        paramValue,
                                           size_t*      paramValueSizeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clGetPipeInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clGetPipeInfo,
        ( pipe, paramName, paramValueSize, paramValue, paramValueSizeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clGetPipeInfo );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clSVMAlloc )( cl_context       context,
                                        cl_svm_mem_flags flags,
                                        size_t           size,
                                        cl_uint          alignment )
{
    SCOREP_OPENCL_FUNC_ENTER( clSVMAlloc );

    SCOREP_OPENCL_WRAP_ENTER();
    void* ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSVMAlloc, ( context, flags, size, alignment ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSVMAlloc );

    return ret;
}



void
SCOREP_LIBWRAP_FUNC_NAME( clSVMFree )( cl_context context,
                                       void*      svmPointer )
{
    SCOREP_OPENCL_FUNC_ENTER( clSVMFree );

    SCOREP_OPENCL_WRAP_ENTER();
    SCOREP_LIBWRAP_FUNC_CALL( clSVMFree, ( context, svmPointer ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSVMFree );
}



cl_sampler
SCOREP_LIBWRAP_FUNC_NAME( clCreateSamplerWithProperties )( cl_context                   context,
                                                           const cl_sampler_properties* normalizedCoords,
                                                           cl_int*                      errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateSamplerWithProperties );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_sampler ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateSamplerWithProperties, ( context, normalizedCoords, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateSamplerWithProperties );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelArgSVMPointer )( cl_kernel   kernel,
                                                      cl_uint     argIndex,
                                                      const void* argValue )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetKernelArgSVMPointer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetKernelArgSVMPointer, ( kernel, argIndex, argValue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetKernelArgSVMPointer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelExecInfo )( cl_kernel           kernel,
                                                 cl_kernel_exec_info paramName,
                                                 size_t              paramValueSize,
                                                 const void*         paramValue )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetKernelExecInfo );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetKernelExecInfo, ( kernel, paramName, paramValueSize, paramValue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetKernelExecInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMFree )( cl_command_queue commandQueue,
                                              cl_uint          numSvmPointers,
                                              void**           svmPointers,
                                              void( CL_CALLBACK * pfnFreeFunc )( cl_command_queue queue,
                                                                                 cl_uint num_svm_pointers,
                                                                                 void**           svm_pointers,
                                                                                 void*            user_data ),
                                              void*            userData,
                                              cl_uint          numEventsInWaitList,
                                              const cl_event*  eventWaitList,
                                              cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueSVMFree );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueSVMFree,
        ( commandQueue, numSvmPointers, svmPointers, pfnFreeFunc, userData,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueSVMFree );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMemcpy )( cl_command_queue commandQueue,
                                                cl_bool          blockingCopy,
                                                void*            dstPtr,
                                                const void*      srcPtr,
                                                size_t           size,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueSVMMemcpy );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueSVMMemcpy,
        ( commandQueue, blockingCopy, dstPtr, srcPtr, size, numEventsInWaitList,
          eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueSVMMemcpy );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMemFill )( cl_command_queue commandQueue,
                                                 void*            svmPtr,
                                                 const void*      pattern,
                                                 size_t           patternSize,
                                                 size_t           size,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueSVMMemFill );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueSVMMemFill,
        ( commandQueue, svmPtr, pattern, patternSize, size, numEventsInWaitList,
          eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueSVMMemFill );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMap )( cl_command_queue commandQueue,
                                             cl_bool          blockingMap,
                                             cl_map_flags     flags,
                                             void*            svmPtr,
                                             size_t           size,
                                             cl_uint          numEventsInWaitList,
                                             const cl_event*  eventWaitList,
                                             cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueSVMMap );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueSVMMap,
        ( commandQueue, blockingMap, flags, svmPtr, size, numEventsInWaitList,
          eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueSVMMap );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMUnmap )( cl_command_queue commandQueue,
                                               void*            svmPtr,
                                               cl_uint          numEventsInWaitList,
                                               const cl_event*  eventWaitList,
                                               cl_event*        event )
{
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueSVMUnmap );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueSVMUnmap,
        ( commandQueue, svmPtr, numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueSVMUnmap );

    return ret;
}
