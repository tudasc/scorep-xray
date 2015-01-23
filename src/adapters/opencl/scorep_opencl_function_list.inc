#ifndef SCOREP_OPENCL_PROCESS_FUNC
#error Macro 'SCOREP_OPENCL_PROCESS_FUNC' was not defined
#endif

/*
 * OpenCL API Version 1.0 Routines
 */
#if HAVE( OPENCL_VERSION_1_0_SUPPORT )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetPlatformIDs,                           ( cl_uint, cl_platform_id *, cl_uint * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetPlatformInfo,                          ( cl_platform_id, cl_platform_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetDeviceIDs,                             ( cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetDeviceInfo,                            ( cl_device_id, cl_device_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_context,         clCreateContext,                            ( const cl_context_properties *, cl_uint, const cl_device_id *, void ( CL_CALLBACK * )( const char *, const void *, size_t, void * ), void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_context,         clCreateContextFromType,                    ( const cl_context_properties *, cl_device_type, void ( CL_CALLBACK * )( const char *, const void *, size_t, void * ), void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainContext,                            ( cl_context ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseContext,                           ( cl_context ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetContextInfo,                           ( cl_context, cl_context_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainCommandQueue,                       ( cl_command_queue ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseCommandQueue,                      ( cl_command_queue ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetCommandQueueInfo,                      ( cl_command_queue, cl_command_queue_info, size_t, void *, size_t * ) )
/* Why is this function not marked as deprecated since OpenCL 2.0? In OpenCL 2.0 it is replaced by clCreateCommandQueueWithProperties. */
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetCommandQueueProperty,                  ( cl_command_queue, cl_command_queue_properties, cl_bool, cl_command_queue_properties * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_mem,             clCreateBuffer,                             ( cl_context, cl_mem_flags, size_t, void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainMemObject,                          ( cl_mem ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseMemObject,                         ( cl_mem ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetSupportedImageFormats,                 ( cl_context, cl_mem_flags, cl_mem_object_type, cl_uint, cl_image_format *, cl_uint * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetMemObjectInfo,                         ( cl_mem, cl_mem_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetImageInfo,                             ( cl_mem, cl_image_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainSampler,                            ( cl_sampler ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseSampler,                           ( cl_sampler ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetSamplerInfo,                           ( cl_sampler, cl_sampler_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_program,         clCreateProgramWithSource,                  ( cl_context, cl_uint, const char **, const size_t *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_program,         clCreateProgramWithBinary,                  ( cl_context, cl_uint, const cl_device_id *, const size_t *, const unsigned char **, cl_int *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainProgram,                            ( cl_program ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseProgram,                           ( cl_program ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clBuildProgram,                             ( cl_program, cl_uint, const cl_device_id *, const char *, void ( CL_CALLBACK * ) ( cl_program , void * ), void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetProgramInfo,                           ( cl_program, cl_program_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetProgramBuildInfo,                      ( cl_program, cl_device_id, cl_program_build_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_kernel,          clCreateKernel,                             ( cl_program, const char *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clCreateKernelsInProgram,                   ( cl_program, cl_uint, cl_kernel *, cl_uint * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainKernel,                             ( cl_kernel ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseKernel,                            ( cl_kernel ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetKernelArg,                             ( cl_kernel, cl_uint, size_t, const void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetKernelInfo,                            ( cl_kernel, cl_kernel_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetKernelWorkGroupInfo,                   ( cl_kernel, cl_device_id, cl_kernel_work_group_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clWaitForEvents,                            ( cl_uint, const cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetEventInfo,                             ( cl_event, cl_event_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainEvent,                              ( cl_event ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseEvent,                             ( cl_event ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetEventProfilingInfo,                    ( cl_event, cl_profiling_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clFlush,                                    ( cl_command_queue ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clFinish,                                   ( cl_command_queue ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueReadBuffer,                        ( cl_command_queue, cl_mem, cl_bool, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueWriteBuffer,                       ( cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueCopyBuffer,                        ( cl_command_queue, cl_mem, cl_mem, size_t, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueReadImage,                         ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueWriteImage,                        ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueCopyImage,                         ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueCopyImageToBuffer,                 ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueCopyBufferToImage,                 ( cl_command_queue, cl_mem, cl_mem, size_t, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( void *,             clEnqueueMapBuffer,                         ( cl_command_queue, cl_mem, cl_bool, cl_map_flags, size_t, size_t, cl_uint, const cl_event *, cl_event *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( void *,             clEnqueueMapImage,                          ( cl_command_queue, cl_mem, cl_bool, cl_map_flags, const size_t *, const size_t *, size_t *, size_t *, cl_uint, const cl_event *, cl_event *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueUnmapMemObject,                    ( cl_command_queue, cl_mem, void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueNDRangeKernel,                     ( cl_command_queue, cl_kernel, cl_uint, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueNativeKernel,                      ( cl_command_queue, void (CL_CALLBACK * )(void *), void *, size_t, cl_uint, const cl_mem *, const void **, cl_uint, const cl_event *, cl_event * ) )

//
// Marked as deprecated APIs since OpenCL 1.1
//
SCOREP_OPENCL_PROCESS_FUNC(  cl_mem,            clCreateImage2D,                            ( cl_context, cl_mem_flags, const cl_image_format *, size_t, size_t, size_t, void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC(  cl_mem,            clCreateImage3D,                            ( cl_context, cl_mem_flags, const cl_image_format *, size_t, size_t, size_t, size_t, size_t, void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC(  cl_int,            clEnqueueMarker,                            ( cl_command_queue, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC(  cl_int,            clEnqueueWaitForEvents,                     ( cl_command_queue, cl_uint,const cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC(  cl_int,            clEnqueueBarrier,                           ( cl_command_queue ) )
SCOREP_OPENCL_PROCESS_FUNC(  cl_int,            clUnloadCompiler,                           ( void ) )
SCOREP_OPENCL_PROCESS_FUNC(  void *,            clGetExtensionFunctionAddress,              ( const char * ) )
//
// Marked as deprecated APIs since OpenCL 2.0
//
SCOREP_OPENCL_PROCESS_FUNC( cl_command_queue,   clCreateCommandQueue,                       ( cl_context, cl_device_id, cl_command_queue_properties, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_sampler,         clCreateSampler,                            ( cl_context, cl_bool, cl_addressing_mode, cl_filter_mode, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueTask,                              ( cl_command_queue, cl_kernel, cl_uint, const cl_event *, cl_event * ) )
#endif

/*
 * OpenCL API Version 1.1 Routines
 */
#if HAVE( OPENCL_VERSION_1_1_SUPPORT )
SCOREP_OPENCL_PROCESS_FUNC( cl_mem,             clCreateSubBuffer,                          ( cl_mem, cl_mem_flags, cl_buffer_create_type, const void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetMemObjectDestructorCallback,           ( cl_mem, void ( CL_CALLBACK * )( cl_mem, void * ), void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_event,           clCreateUserEvent,                          ( cl_context, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetUserEventStatus,                       ( cl_event, cl_int ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetEventCallback,                         ( cl_event, cl_int, void ( CL_CALLBACK * ) ( cl_event, cl_int, void * ), void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueReadBufferRect,                    ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueWriteBufferRect,                   ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueCopyBufferRect,                    ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, cl_uint , const cl_event *, cl_event * ) )
#endif

/*
 * OpenCL API Version 1.2 Routines
 */
#if HAVE( OPENCL_VERSION_1_2_SUPPORT )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clCreateSubDevices,                         ( cl_device_id, const cl_device_partition_property *, cl_uint, cl_device_id *, cl_uint * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clRetainDevice,                             ( cl_device_id ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clReleaseDevice,                            ( cl_device_id ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_mem,             clCreateImage,                              ( cl_context, cl_mem_flags, const cl_image_format *, const cl_image_desc *, void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_program,         clCreateProgramWithBuiltInKernels,          ( cl_context, cl_uint, const cl_device_id *, const char *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clCompileProgram,                           ( cl_program, cl_uint, const cl_device_id *, const char *, cl_uint, const cl_program *, const char **, void ( CL_CALLBACK * ) ( cl_program, void * ), void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_program,         clLinkProgram,                              ( cl_context, cl_uint, const cl_device_id *, const char *, cl_uint, const cl_program *, void ( CL_CALLBACK * ) ( cl_program, void * ), void *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clUnloadPlatformCompiler,                   ( cl_platform_id ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetKernelArgInfo,                         ( cl_kernel, cl_uint, cl_kernel_arg_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueFillBuffer,                        ( cl_command_queue, cl_mem, const void *, size_t, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueFillImage,                         ( cl_command_queue, cl_mem, const void *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueMigrateMemObjects,                 ( cl_command_queue, cl_uint, const cl_mem *, cl_mem_migration_flags, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueMarkerWithWaitList,                ( cl_command_queue, cl_uint, const cl_event *,cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueBarrierWithWaitList,               ( cl_command_queue, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( void *,             clGetExtensionFunctionAddressForPlatform,   ( cl_platform_id, const char * ) )

#endif

/*
 * OpenCL API Version 2.0 Routines
 */
#if HAVE( OPENCL_VERSION_2_0_SUPPORT )
SCOREP_OPENCL_PROCESS_FUNC( cl_command_queue,   clCreateCommandQueueWithProperties,         ( cl_context, cl_device_id, const cl_queue_properties *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_mem,             clCreatePipe,                               ( cl_context, cl_mem_flags, cl_uint, cl_uint, const cl_pipe_properties *,  cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clGetPipeInfo,                              ( cl_mem, cl_pipe_info, size_t, void *, size_t * ) )
SCOREP_OPENCL_PROCESS_FUNC( void *,             clSVMAlloc,                                 ( cl_context, cl_svm_mem_flags, size_t, cl_uint ) )
SCOREP_OPENCL_PROCESS_FUNC( void,               clSVMFree,                                  ( cl_context, void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_sampler,         clCreateSamplerWithProperties,              ( cl_context, const cl_sampler_properties *, cl_int * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetKernelArgSVMPointer,                   ( cl_kernel, cl_uint, const void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clSetKernelExecInfo,                        ( cl_kernel, cl_kernel_exec_info, size_t, const void * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueSVMFree,                           ( cl_command_queue, cl_uint, void *[], void ( CL_CALLBACK * )( cl_command_queue, cl_uint, void *[], void * ), void *, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueSVMMemcpy,                         ( cl_command_queue, cl_bool, void *, const void *, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueSVMMemFill,                        ( cl_command_queue, void *, const void *, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueSVMMap,                            ( cl_command_queue, cl_bool, cl_map_flags, void *, size_t, cl_uint, const cl_event *, cl_event * ) )
SCOREP_OPENCL_PROCESS_FUNC( cl_int,             clEnqueueSVMUnmap,                          ( cl_command_queue, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#undef SCOREP_OPENCL_PROCESS_FUNC
