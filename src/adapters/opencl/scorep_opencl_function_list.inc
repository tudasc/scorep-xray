#ifndef SCOREP_OPENCL_PROCESS_FUNC
#error Macro 'SCOREP_OPENCL_PROCESS_FUNC' was not defined
#endif

/*
 * OpenCL API Version 1.0 Routines
 */
#if HAVE( OPENCL_VERSION_1_0_SUPPORT )

#if HAVE( OPENCL_1_0_SYMBOL_CLGETPLATFORMIDS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetPlatformIDs,                           ( cl_uint, cl_platform_id *, cl_uint * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETPLATFORMINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetPlatformInfo,                          ( cl_platform_id, cl_platform_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETDEVICEIDS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetDeviceIDs,                             ( cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETDEVICEINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetDeviceInfo,                            ( cl_device_id, cl_device_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATECONTEXT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_context,         clCreateContext,                            ( const cl_context_properties *, cl_uint, const cl_device_id *, void ( CL_CALLBACK * )( const char *, const void *, size_t, void * ), void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATECONTEXTFROMTYPE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_context,         clCreateContextFromType,                    ( const cl_context_properties *, cl_device_type, void ( CL_CALLBACK * )( const char *, const void *, size_t, void * ), void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINCONTEXT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainContext,                            ( cl_context ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASECONTEXT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseContext,                           ( cl_context ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETCONTEXTINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetContextInfo,                           ( cl_context, cl_context_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINCOMMANDQUEUE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainCommandQueue,                       ( cl_command_queue ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASECOMMANDQUEUE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseCommandQueue,                      ( cl_command_queue ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETCOMMANDQUEUEINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetCommandQueueInfo,                      ( cl_command_queue, cl_command_queue_info, size_t, void *, size_t * ) )
#endif

/* Why is this function not marked as deprecated since OpenCL 2.0? In OpenCL 2.0 it is replaced by clCreateCommandQueueWithProperties. */
#if HAVE( OPENCL_1_0_SYMBOL_CLSETCOMMANDQUEUEPROPERTY )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetCommandQueueProperty,                  ( cl_command_queue, cl_command_queue_properties, cl_bool, cl_command_queue_properties * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_mem,             clCreateBuffer,                             ( cl_context, cl_mem_flags, size_t, void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINMEMOBJECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainMemObject,                          ( cl_mem ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEMEMOBJECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseMemObject,                         ( cl_mem ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETSUPPORTEDIMAGEFORMATS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetSupportedImageFormats,                 ( cl_context, cl_mem_flags, cl_mem_object_type, cl_uint, cl_image_format *, cl_uint * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETMEMOBJECTINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetMemObjectInfo,                         ( cl_mem, cl_mem_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETIMAGEINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetImageInfo,                             ( cl_mem, cl_image_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINSAMPLER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainSampler,                            ( cl_sampler ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASESAMPLER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseSampler,                           ( cl_sampler ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETSAMPLERINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetSamplerInfo,                           ( cl_sampler, cl_sampler_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEPROGRAMWITHSOURCE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_program,         clCreateProgramWithSource,                  ( cl_context, cl_uint, const char **, const size_t *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEPROGRAMWITHBINARY )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_program,         clCreateProgramWithBinary,                  ( cl_context, cl_uint, const cl_device_id *, const size_t *, const unsigned char **, cl_int *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainProgram,                            ( cl_program ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseProgram,                           ( cl_program ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLBUILDPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clBuildProgram,                             ( cl_program, cl_uint, const cl_device_id *, const char *, void ( CL_CALLBACK * ) ( cl_program , void * ), void * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETPROGRAMINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetProgramInfo,                           ( cl_program, cl_program_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETPROGRAMBUILDINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetProgramBuildInfo,                      ( cl_program, cl_device_id, cl_program_build_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_kernel,          clCreateKernel,                             ( cl_program, const char *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEKERNELSINPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clCreateKernelsInProgram,                   ( cl_program, cl_uint, cl_kernel *, cl_uint * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainKernel,                             ( cl_kernel ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseKernel,                            ( cl_kernel ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLSETKERNELARG )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetKernelArg,                             ( cl_kernel, cl_uint, size_t, const void * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETKERNELINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetKernelInfo,                            ( cl_kernel, cl_kernel_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETKERNELWORKGROUPINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetKernelWorkGroupInfo,                   ( cl_kernel, cl_device_id, cl_kernel_work_group_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLWAITFOREVENTS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clWaitForEvents,                            ( cl_uint, const cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETEVENTINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetEventInfo,                             ( cl_event, cl_event_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRETAINEVENT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainEvent,                              ( cl_event ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLRELEASEEVENT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseEvent,                             ( cl_event ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETEVENTPROFILINGINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetEventProfilingInfo,                    ( cl_event, cl_profiling_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLFLUSH )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clFlush,                                    ( cl_command_queue ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLFINISH )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clFinish,                                   ( cl_command_queue ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEREADBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueReadBuffer,                        ( cl_command_queue, cl_mem, cl_bool, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWRITEBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueWriteBuffer,                       ( cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueCopyBuffer,                        ( cl_command_queue, cl_mem, cl_mem, size_t, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEREADIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueReadImage,                         ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWRITEIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueWriteImage,                        ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueCopyImage,                         ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYIMAGETOBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueCopyImageToBuffer,                 ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUECOPYBUFFERTOIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueCopyBufferToImage,                 ( cl_command_queue, cl_mem, cl_mem, size_t, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMAPBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, void *,             clEnqueueMapBuffer,                         ( cl_command_queue, cl_mem, cl_bool, cl_map_flags, size_t, size_t, cl_uint, const cl_event *, cl_event *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMAPIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, void *,             clEnqueueMapImage,                          ( cl_command_queue, cl_mem, cl_bool, cl_map_flags, const size_t *, const size_t *, size_t *, size_t *, cl_uint, const cl_event *, cl_event *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEUNMAPMEMOBJECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueUnmapMemObject,                    ( cl_command_queue, cl_mem, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUENDRANGEKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( KERNEL_LAUNCH, cl_int,       clEnqueueNDRangeKernel,                     ( cl_command_queue, cl_kernel, cl_uint, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUENATIVEKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueNativeKernel,                      ( cl_command_queue, void (CL_CALLBACK * )(void *), void *, size_t, cl_uint, const cl_mem *, const void **, cl_uint, const cl_event *, cl_event * ) )
#endif

//
// Marked as deprecated APIs since OpenCL 1.1
//
#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEIMAGE2D )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_mem,            clCreateImage2D,                            ( cl_context, cl_mem_flags, const cl_image_format *, size_t, size_t, size_t, void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATEIMAGE3D )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_mem,            clCreateImage3D,                            ( cl_context, cl_mem_flags, const cl_image_format *, size_t, size_t, size_t, size_t, size_t, void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEMARKER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_int,            clEnqueueMarker,                            ( cl_command_queue, cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEWAITFOREVENTS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_int,            clEnqueueWaitForEvents,                     ( cl_command_queue, cl_uint,const cl_event * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUEBARRIER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_int,            clEnqueueBarrier,                           ( cl_command_queue ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLUNLOADCOMPILER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  cl_int,            clUnloadCompiler,                           ( void ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLGETEXTENSIONFUNCTIONADDRESS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER,  void *,            clGetExtensionFunctionAddress,              ( const char * ) )
#endif

//
// Marked as deprecated APIs since OpenCL 2.0
//
#if HAVE( OPENCL_1_0_SYMBOL_CLCREATECOMMANDQUEUE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_command_queue,   clCreateCommandQueue,                       ( cl_context, cl_device_id, cl_command_queue_properties, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLCREATESAMPLER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_sampler,         clCreateSampler,                            ( cl_context, cl_bool, cl_addressing_mode, cl_filter_mode, cl_int * ) )
#endif

#if HAVE( OPENCL_1_0_SYMBOL_CLENQUEUETASK )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueTask,                              ( cl_command_queue, cl_kernel, cl_uint, const cl_event *, cl_event * ) )
#endif

#endif

/*
 * OpenCL API Version 1.1 Routines
 */
#if HAVE( OPENCL_VERSION_1_1_SUPPORT )
#if HAVE( OPENCL_1_1_SYMBOL_CLCREATESUBBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_mem,             clCreateSubBuffer,                          ( cl_mem, cl_mem_flags, cl_buffer_create_type, const void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLSETMEMOBJECTDESTRUCTORCALLBACK )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetMemObjectDestructorCallback,           ( cl_mem, void ( CL_CALLBACK * )( cl_mem, void * ), void * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLCREATEUSEREVENT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_event,           clCreateUserEvent,                          ( cl_context, cl_int * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLSETUSEREVENTSTATUS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetUserEventStatus,                       ( cl_event, cl_int ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLSETEVENTCALLBACK )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetEventCallback,                         ( cl_event, cl_int, void ( CL_CALLBACK * ) ( cl_event, cl_int, void * ), void * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLENQUEUEREADBUFFERRECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueReadBufferRect,                    ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLENQUEUEWRITEBUFFERRECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueWriteBufferRect,                   ( cl_command_queue, cl_mem, cl_bool, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_1_SYMBOL_CLENQUEUECOPYBUFFERRECT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueCopyBufferRect,                    ( cl_command_queue, cl_mem, cl_mem, const size_t *, const size_t *, const size_t *, size_t, size_t, size_t, size_t, cl_uint , const cl_event *, cl_event * ) )
#endif

#endif

/*
 * OpenCL API Version 1.2 Routines
 */
#if HAVE( OPENCL_VERSION_1_2_SUPPORT )

#if HAVE( OPENCL_1_2_SYMBOL_CLCREATESUBDEVICES )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clCreateSubDevices,                         ( cl_device_id, const cl_device_partition_property *, cl_uint, cl_device_id *, cl_uint * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLRETAINDEVICE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clRetainDevice,                             ( cl_device_id ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLRELEASEDEVICE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clReleaseDevice,                            ( cl_device_id ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLCREATEIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_mem,             clCreateImage,                              ( cl_context, cl_mem_flags, const cl_image_format *, const cl_image_desc *, void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLCREATEPROGRAMWITHBUILTINKERNELS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_program,         clCreateProgramWithBuiltInKernels,          ( cl_context, cl_uint, const cl_device_id *, const char *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLCOMPILEPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clCompileProgram,                           ( cl_program, cl_uint, const cl_device_id *, const char *, cl_uint, const cl_program *, const char **, void ( CL_CALLBACK * ) ( cl_program, void * ), void * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLLINKPROGRAM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_program,         clLinkProgram,                              ( cl_context, cl_uint, const cl_device_id *, const char *, cl_uint, const cl_program *, void ( CL_CALLBACK * ) ( cl_program, void * ), void *, cl_int * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLUNLOADPLATFORMCOMPILER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clUnloadPlatformCompiler,                   ( cl_platform_id ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLGETKERNELARGINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetKernelArgInfo,                         ( cl_kernel, cl_uint, cl_kernel_arg_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLENQUEUEFILLBUFFER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueFillBuffer,                        ( cl_command_queue, cl_mem, const void *, size_t, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLENQUEUEFILLIMAGE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueFillImage,                         ( cl_command_queue, cl_mem, const void *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLENQUEUEMIGRATEMEMOBJECTS )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueMigrateMemObjects,                 ( cl_command_queue, cl_uint, const cl_mem *, cl_mem_migration_flags, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLENQUEUEMARKERWITHWAITLIST )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueMarkerWithWaitList,                ( cl_command_queue, cl_uint, const cl_event *,cl_event * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLENQUEUEBARRIERWITHWAITLIST )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueBarrierWithWaitList,               ( cl_command_queue, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_1_2_SYMBOL_CLGETEXTENSIONFUNCTIONADDRESSFORPLATFORM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, void *,             clGetExtensionFunctionAddressForPlatform,   ( cl_platform_id, const char * ) )
#endif


#endif

/*
 * OpenCL API Version 2.0 Routines
 */
#if HAVE( OPENCL_VERSION_2_0_SUPPORT )

#if HAVE( OPENCL_2_0_SYMBOL_CLCREATECOMMANDQUEUEWITHPROPERTIES )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_command_queue,   clCreateCommandQueueWithProperties,         ( cl_context, cl_device_id, const cl_queue_properties *, cl_int * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLCREATEPIPE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_mem,             clCreatePipe,                               ( cl_context, cl_mem_flags, cl_uint, cl_uint, const cl_pipe_properties *,  cl_int * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLGETPIPEINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetPipeInfo,                              ( cl_mem, cl_pipe_info, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLSVMALLOC )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, void *,             clSVMAlloc,                                 ( cl_context, cl_svm_mem_flags, size_t, cl_uint ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLSVMFREE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, void,               clSVMFree,                                  ( cl_context, void * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLCREATESAMPLERWITHPROPERTIES )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_sampler,         clCreateSamplerWithProperties,              ( cl_context, const cl_sampler_properties *, cl_int * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLSETKERNELARGSVMPOINTER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetKernelArgSVMPointer,                   ( cl_kernel, cl_uint, const void * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLSETKERNELEXECINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetKernelExecInfo,                        ( cl_kernel, cl_kernel_exec_info, size_t, const void * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLENQUEUESVMFREE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMFree,                           ( cl_command_queue, cl_uint, void *[], void ( CL_CALLBACK * )( cl_command_queue, cl_uint, void *[], void * ), void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLENQUEUESVMMEMCPY )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMMemcpy,                         ( cl_command_queue, cl_bool, void *, const void *, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLENQUEUESVMMEMFILL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMMemFill,                        ( cl_command_queue, void *, const void *, size_t, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLENQUEUESVMMAP )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMMap,                            ( cl_command_queue, cl_bool, cl_map_flags, void *, size_t, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_2_0_SYMBOL_CLENQUEUESVMUNMAP )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMUnmap,                          ( cl_command_queue, void *, cl_uint, const cl_event *, cl_event * ) )
#endif

#endif

/*
 * OpenCL API Version 2.1 Routines
 */
#if HAVE( OPENCL_VERSION_2_1_SUPPORT )

#if HAVE( OPENCL_2_1_SYMBOL_CLGETKERNELSUBGROUPINFO )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetKernelSubGroupInfo,                    ( cl_kernel, cl_device_id, cl_kernel_sub_group_info, size_t, const void *, size_t, void *, size_t * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLCREATEPROGRAMWITHIL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_program,         clCreateProgramWithIL,                      ( cl_context, const void *, size_t, cl_int * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLGETHOSTTIMER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetHostTimer,                             ( cl_device_id, cl_ulong * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLGETDEVICEANDHOSTTIMER )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clGetDeviceAndHostTimer,                    ( cl_device_id, cl_ulong *, cl_ulong * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLENQUEUESVMMIGRATEMEM )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clEnqueueSVMMigrateMem,                     ( cl_command_queue, cl_uint, const void **, const size_t *, cl_mem_migration_flags, cl_uint, const cl_event *, cl_event * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLCLONEKERNEL )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_kernel,          clCloneKernel,                              ( cl_kernel, cl_int * ) )
#endif

#if HAVE( OPENCL_2_1_SYMBOL_CLSETDEFAULTDEVICECOMMANDQUEUE )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetDefaultDeviceCommandQueue,             ( cl_context, cl_device_id, cl_command_queue ) )
#endif

#endif

/*
 * OpenCL API Version 2.2 Routines
 */
#if HAVE( OPENCL_VERSION_2_2_SUPPORT )

#if HAVE( OPENCL_2_2_SYMBOL_CLSETPROGRAMSPECIALIZATIONCONSTANT )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetProgramSpecializationConstant,         ( cl_program, cl_uint, size_t, const void * ) )
#endif

#if HAVE( OPENCL_2_2_SYMBOL_CLSETPROGRAMRELEASECALLBACK )
SCOREP_OPENCL_PROCESS_FUNC( WRAPPER, cl_int,             clSetProgramReleaseCallback,                ( cl_program, void (CL_CALLBACK*)(cl_program, void * ), void * ) )
#endif

#endif

#undef SCOREP_OPENCL_PROCESS_FUNC
