/* *INDENT-OFF* */

cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateSubBuffer ) ( cl_mem                buffer,
                                                cl_mem_flags          flags,
                                                cl_buffer_create_type bufferCreateType,
                                                const void*           bufferCreateInfo,
                                                cl_int*               errcodeRet )
{
    cl_mem ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateSubBuffer );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateSubBuffer,
                                             clCreateSubBuffer,
                                             ( buffer, flags, bufferCreateType,
                                               bufferCreateInfo, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateSubBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetMemObjectDestructorCallback ) ( cl_mem memObj,
                                                               void ( CL_CALLBACK* pfnNotify )( cl_mem,
                                                                                                void* ),
                                                               void*  userData )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetMemObjectDestructorCallback );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetMemObjectDestructorCallback,
                                             clSetMemObjectDestructorCallback,
                                             ( memObj, pfnNotify, userData ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetMemObjectDestructorCallback );

    return ret;
}



cl_event
SCOREP_LIBWRAP_FUNC_NAME( clCreateUserEvent ) ( cl_context context,
                                                cl_int*    errcodeRet )
{
    cl_event ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clCreateUserEvent );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clCreateUserEvent,
                                             clCreateUserEvent,
                                             ( context, errcodeRet ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clCreateUserEvent );

    return ret;
}




cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetUserEventStatus ) ( cl_event event,
                                                   cl_int   executionStatus )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetUserEventStatus );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetUserEventStatus,
                                             clSetUserEventStatus,
                                             ( event, executionStatus ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetUserEventStatus );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetEventCallback ) ( cl_event event,
                                                 cl_int   commandExecCallbackType,
                                                 void ( CL_CALLBACK* pfnNotify )( cl_event,
                                                                                  cl_int,
                                                                                  void* ),
                                                 void*    userData )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clSetEventCallback );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clSetEventCallback,
                                             clSetEventCallback,
                                             ( event, commandExecCallbackType,
                                               pfnNotify, userData ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clSetEventCallback );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadBufferRect ) ( cl_command_queue commandQueue,
                                                      cl_mem           buffer,
                                                      cl_bool          blockingRead,
                                                      const size_t*    bufferOffset,
                                                      const size_t*    hostOffset,
                                                      const size_t*    region,
                                                      size_t           bufferRowPitch,
                                                      size_t           bufferSlicePitch,
                                                      size_t           hostRowPitch,
                                                      size_t           hostSlicePitch,
                                                      void*            ptr,
                                                      cl_uint          numEventsInWaitList,
                                                      const cl_event*  eventWaitList,
                                                      cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueReadBufferRect );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueReadBufferRect,
                                             clEnqueueReadBufferRect,
                                             ( commandQueue, buffer, blockingRead,
                                               bufferOffset, hostOffset, region,
                                               bufferRowPitch, bufferSlicePitch,
                                               hostRowPitch, hostSlicePitch, ptr,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueReadBufferRect );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteBufferRect ) ( cl_command_queue commandQueue,
                                                       cl_mem           buffer,
                                                       cl_bool          blockingWrite,
                                                       const size_t*    bufferOffset,
                                                       const size_t*    hostOffset,
                                                       const size_t*    region,
                                                       size_t           bufferRowPitch,
                                                       size_t           bufferSlicePitch,
                                                       size_t           hostRowPitch,
                                                       size_t           hostSlicePitch,
                                                       const void*      ptr,
                                                       cl_uint          numEventsInWaitList,
                                                       const cl_event*  eventWaitList,
                                                       cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueWriteBufferRect );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueWriteBufferRect,
                                             clEnqueueWriteBufferRect,
                                             ( commandQueue, buffer, blockingWrite,
                                               bufferOffset, hostOffset, region,
                                               bufferRowPitch, bufferSlicePitch,
                                               hostRowPitch, hostSlicePitch, ptr,
                                               numEventsInWaitList, eventWaitList,
                                               event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueWriteBufferRect );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBufferRect ) ( cl_command_queue commandQueue,
                                                      cl_mem           srcBuffer,
                                                      cl_mem           dstBuffer,
                                                      const size_t*    srcOrigin,
                                                      const size_t*    dstOrigin,
                                                      const size_t*    region,
                                                      size_t           srcRowPitch,
                                                      size_t           srcSlicePitch,
                                                      size_t           dstRowPitch,
                                                      size_t           dstSlicePitch,
                                                      cl_uint          numEventsInWaitList,
                                                      const cl_event*  eventWaitList,
                                                      cl_event*        event )
{
    cl_int ret;

    SCOREP_OPENCL_FUNC_ENTER( scorep_opencl_region__clEnqueueCopyBufferRect );

    ret = SCOREP_LIBWRAP_INTERNAL_FUNC_CALL( scorep_opencl_funcptr_clEnqueueCopyBufferRect,
                                             clEnqueueCopyBufferRect,
                                             ( commandQueue, srcBuffer, dstBuffer, srcOrigin,
                                               dstOrigin, region, srcRowPitch, srcSlicePitch,
                                               dstRowPitch, dstSlicePitch, numEventsInWaitList,
                                               eventWaitList, event ) );

    SCOREP_OPENCL_FUNC_EXIT( scorep_opencl_region__clEnqueueCopyBufferRect );

    return ret;
}

/* *INDENT-ON* */
