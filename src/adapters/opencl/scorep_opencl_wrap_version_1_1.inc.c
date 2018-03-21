cl_mem
SCOREP_LIBWRAP_FUNC_NAME( clCreateSubBuffer )( cl_mem                buffer,
                                               cl_mem_flags          flags,
                                               cl_buffer_create_type bufferCreateType,
                                               const void*           bufferCreateInfo,
                                               cl_int*               errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateSubBuffer );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_mem ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateSubBuffer,
        ( buffer, flags, bufferCreateType, bufferCreateInfo, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateSubBuffer );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetMemObjectDestructorCallback )( cl_mem memObj,
                                                              void( CL_CALLBACK * pfnNotify )( cl_mem,
                                                                                               void* ),
                                                              void*  userData )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetMemObjectDestructorCallback );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetMemObjectDestructorCallback, ( memObj, pfnNotify, userData ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetMemObjectDestructorCallback );

    return ret;
}



cl_event
SCOREP_LIBWRAP_FUNC_NAME( clCreateUserEvent )( cl_context context,
                                               cl_int*    errcodeRet )
{
    SCOREP_OPENCL_FUNC_ENTER( clCreateUserEvent );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_event ret = SCOREP_LIBWRAP_FUNC_CALL(
        clCreateUserEvent, ( context, errcodeRet ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clCreateUserEvent );

    return ret;
}




cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetUserEventStatus )( cl_event event,
                                                  cl_int   executionStatus )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetUserEventStatus );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetUserEventStatus, ( event, executionStatus ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetUserEventStatus );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetEventCallback )( cl_event event,
                                                cl_int   commandExecCallbackType,
                                                void( CL_CALLBACK * pfnNotify )( cl_event,
                                                                                 cl_int,
                                                                                 void* ),
                                                void*    userData )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetEventCallback );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetEventCallback,
        ( event, commandExecCallbackType, pfnNotify, userData ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetEventCallback );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueReadBufferRect )( cl_command_queue commandQueue,
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
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueReadBufferRect );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueReadBufferRect,
        ( commandQueue, buffer, blockingRead, bufferOffset, hostOffset, region,
          bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, ptr,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueReadBufferRect );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueWriteBufferRect )( cl_command_queue commandQueue,
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
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueWriteBufferRect );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueWriteBufferRect,
        ( commandQueue, buffer, blockingWrite, bufferOffset, hostOffset, region,
          bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, ptr,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueWriteBufferRect );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clEnqueueCopyBufferRect )( cl_command_queue commandQueue,
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
    SCOREP_OPENCL_FUNC_ENTER( clEnqueueCopyBufferRect );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clEnqueueCopyBufferRect,
        ( commandQueue, srcBuffer, dstBuffer, srcOrigin, dstOrigin, region,
          srcRowPitch, srcSlicePitch, dstRowPitch, dstSlicePitch,
          numEventsInWaitList, eventWaitList, event ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clEnqueueCopyBufferRect );

    return ret;
}
