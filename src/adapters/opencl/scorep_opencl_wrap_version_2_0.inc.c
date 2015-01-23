/* *INDENT-OFF* */

cl_command_queue
SCOREP_LIBWRAP_FUNC_NAME( clCreateCommandQueueWithProperties ) ( cl_context                 context,
                                                                 cl_device_id               device,
                                                                 const cl_queue_properties* properties,
                                                                 cl_int*                    errcodeRet )
{
    cl_command_queue ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateCommandQueueWithProperties );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateCommandQueueWithProperties,
                                             clCreateCommandQueueWithProperties,
                                             ( context, device, properties,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateCommandQueueWithProperties );

    return ret;
}



cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreatePipe ) ( cl_context                context,
                                           cl_mem_flags              flags,
                                           cl_uint                   pipePacketSize,
                                           cl_uint                   pipeMaxPackets,
                                           const cl_pipe_properties* properties,
                                           cl_int*                   errcodeRet )
{
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreatePipe );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreatePipe,
                                             clCreatePipe,
                                             ( context, flags, pipePacketSize,
                                               pipeMaxPackets, properties,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreatePipe );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clGetPipeInfo ) ( cl_mem       pipe,
                                            cl_pipe_info paramName,
                                            size_t       paramValueSize,
                                            void*        paramValue,
                                            size_t*      paramValueSizeRet )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clGetPipeInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clGetPipeInfo,
                                             clGetPipeInfo,
                                             ( pipe, paramName, paramValueSize,
                                               paramValue, paramValueSizeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clGetPipeInfo );

    return ret;
}



void*
SCOREP_LIBWRAP_FUNC_NAME( clSVMAlloc ) ( cl_context       context,
                                         cl_svm_mem_flags flags,
                                         size_t           size,
                                         cl_uint          alignment )
{
    void* ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSVMAlloc );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSVMAlloc,
                                             clSVMAlloc,
                                             ( context, flags, size, alignment ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSVMAlloc );

    return ret;
}



void
SCOREP_LIBWRAP_FUNC_NAME( clSVMFree ) ( cl_context context,
                                        void*      svmPointer )
{
    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSVMFree );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSVMFree,
                                             clSVMFree,
                                             ( context, svmPointer ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSVMFree );
}



cl_sampler
SCOREP_LIBWRAP_FUNC_NAME( clCreateSamplerWithProperties ) ( cl_context                   context,
                                                            const cl_sampler_properties* normalizedCoords,
                                                            cl_int*                      errcodeRet )
{
    cl_sampler ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateSamplerWithProperties );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateSamplerWithProperties,
                                             clCreateSamplerWithProperties,
                                             ( context, normalizedCoords,
                                               errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateSamplerWithProperties );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelArgSVMPointer ) ( cl_kernel   kernel,
                                                       cl_uint     argIndex,
                                                       const void* argValue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetKernelArgSVMPointer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetKernelArgSVMPointer,
                                             clSetKernelArgSVMPointer,
                                             ( kernel, argIndex, argValue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetKernelArgSVMPointer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetKernelExecInfo ) ( cl_kernel           kernel,
                                                  cl_kernel_exec_info paramName,
                                                  size_t              paramValueSize,
                                                  const void*         paramValue )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetKernelExecInfo );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetKernelExecInfo,
                                             clSetKernelExecInfo,
                                             ( kernel, paramName, paramvalueSize,
                                               paramValue ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetKernelExecInfo );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMFree ) ( cl_command_queue commandQueue,
                                               cl_uint          numSvmPointers,
                                               void*[]          svmPointers,
                                               void ( CL_CALLBACK* pfnFreeFunc )( cl_command_queue queue,
                                                                                  cl_uint          num_svm_pointers,
                                                                                  void*[]          svm_pointers,
                                                                                  void*            user_data ),
                                               void*            userData,
                                               cl_uint          numEventsInWaitList,
                                               const cl_event*  eventWaitList,
                                               cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueSVMFree );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueSVMFree,
                                             clEnqueueSVMFree,
                                             ( commandQueue, numSvmPointers,
                                               svmPointers, pfnFreeFunc, userData,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueSVMFree );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMemcpy ) ( cl_command_queue commandQueue,
                                                 cl_bool          blockingCopy,
                                                 void*            dstPtr,
                                                 const void*      srcPtr,
                                                 size_t           size,
                                                 cl_uint          numEventsInWaitList,
                                                 const cl_event*  eventWaitList,
                                                 cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueSVMMemcpy );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueSVMMemcpy,
                                             clEnqueueSVMMemcpy,
                                             ( commandQueue, blockingCopy,
                                               dstPtr, srcPtr, size,
                                               numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueSVMMemcpy );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMemFill ) ( cl_command_queue commandQueue,
                                                  void*            svmPtr,
                                                  const void*      pattern,
                                                  size_t           patternSize,
                                                  size_t           size,
                                                  cl_uint          numEventsInWaitList,
                                                  const cl_event*  eventWaitList,
                                                  cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueSVMMemFill );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueSVMMemFill,
                                             clEnqueueSVMMemFill,
                                             ( commandQueue, svmPtr, pattern,
                                               patternSize, size,
                                               numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueSVMMemFill );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMMap ) ( cl_command_queue commandQueue,
                                              cl_bool          blockingMap,
                                              cl_map_flags     flags,
                                              void*            svmPtr,
                                              size_t           size,
                                              cl_uint          numEventsInWaitList,
                                              const cl_event*  eventWaitList,
                                              cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueSVMMap );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueSVMMap,
                                             clEnqueueSVMMap,
                                             ( commandQueue, blockingMap, flags,
                                               svmPtr, size, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueSVMMap );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueSVMUnmap ) ( cl_command_queue commandQueue,
                                                void*            svmPtr,
                                                cl_uint          numEventsInWaitList,
                                                const cl_event*  eventWaitList,
                                                cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueSVMUnmap );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueSVMUnmap,
                                             clEnqueueSVMUnmap,
                                             ( commandQueue, svmPtr,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueSVMUnmap );

    return ret;
}

/* *INDENT-ON* */
