<metrics>
    <metric>
        <!-- This metric is copied from the summary profile -->
        <disp_name>Time</disp_name>
        <uniq_name>time</uniq_name>
        <dtype>FLOAT</dtype>
        <uom>sec</uom>
        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#time</url>
        <descr>Total CPU allocation time</descr>
        <metric type="PREDERIVED_EXCLUSIVE">
            <disp_name>Execution</disp_name>
            <uniq_name>execution</uniq_name>
            <dtype>FLOAT</dtype>
            <uom>sec</uom>
            <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#execution</url>
            <descr>Total execution time</descr>
            <cubepl>
                ${execution}[${calculation::callpath::id}] * metric::time(e)
            </cubepl>
            <cubeplinit>
            {
                //--- Global callpath type masks ----------------------------
                // Each type mask field contains one entry for each callpath,
                // by default set to 0.  It is set to 1 if the callpath is of
                // the corresponding category.
                //

                // Generic masks
                global(execution);
                global(overhead);

                // MPI-specific masks
                global(mpi_sync_barrier);
                global(mpi_sync_active_target);
                global(mpi_sync_passive_target);
                global(mpi_comm_p2p);
                global(mpi_comm_coll);
                global(mpi_comm_1sided);
                global(mpi_mgmt_startup);
                global(mpi_mgmt_comm);
                global(mpi_io_noncoll);
                global(mpi_io_coll);

                // OpenMP-specific masks
                global(omp_sync_ebarrier);
                global(omp_sync_ibarrier);
                global(omp_sync_critical);
                global(omp_sync_lock_api);
                global(omp_sync_flush);
                global(omp_sync_ordered);

                // Pthread-specific masks
                global(pthread_sync_mutex_api);
                global(pthread_sync_cond_api);
                global(pthread_mgmt);

                // OpenCL-spacific masks
                global(ocl_comp);
                global(ocl_sync);
                global(ocl_comm);
                global(ocl_mgmt);

                // CUDA-specific masks
                global(cuda_comp);
                global(cuda_sync);
                global(cuda_comm);
                global(cuda_mgmt);

                //--- Paradigm flags ----------------------------------------
                // These flags indicate which of the supported parallel
                // programming paradigms are used in the experiment to
                // later enable/disable the corresponding metric subtrees.

                ${includesMPI}     = 0;
                ${includesOpenMP}  = 0;
                ${includesPthread} = 0;
                ${includesOpenCL}  = 0;
                ${includesCUDA}  = 0;


                //--- Callpath categorization -------------------------------
                // Each callpath is categorized by type and its global type
                // mask entry is set accordingly.

                ${i} = 0;
                while ( ${i} < ${cube::#callpaths} )
                {
                    ${regionid} = ${cube::callpath::calleeid}[${i}];
                    ${name}     = ${cube::region::name}[${regionid}];
                    ${paradigm} = ${cube::region::paradigm}[${regionid}];
                    ${role}     = ${cube::region::role}[${regionid}];

                    // Default: all callpaths are considered to be execution
                    ${execution}[${i}] = 1;


                    //--- MPI-specific categorization ---
                    if ( ${paradigm} eq "mpi" )
                    {
                        ${includesMPI} = 1;

                        //--- Synchronization

                        if ( ${role} eq "barrier" )
                        {
                            ${mpi_sync_barrier}[${i}] = 1;
                        };
                        if ( ${name} =~ /^MPI_Win_(post|wait|start|complete|fence)$/ )
                        {
                            ${mpi_sync_active_target}[${i}] = 1;
                        };
                        if ( ${name} =~ /^MPI_Win_(lock|unlock)$/ )
                        {
                            ${mpi_sync_passive_target}[${i}] = 1;
                        };

                        //--- Communication

                        if (
                            ( ${role} eq "point2point" )
                            or
                            ( ${name} =~ /^MPI_.*(buffer|cancel|get_count|request)/ )
                           )
                        {
                            ${mpi_comm_p2p}[${i}] = 1;
                        };
                        if ( ${role} =~ /^(one2all|all2one|all2all|other collective)$/ )
                        {
                            ${mpi_comm_coll}[${i}] = 1;
                        };
                        if ( ${name} =~ /^MPI_(Put|Get|Accumulate)$/ )
                        {
                            ${mpi_comm_1sided}[${i}] = 1;
                        };

                        //--- Management
                        
                        if ( ${name} =~ /^MPI_(Init|Init_thread|Finalize)$/ )
                        {
                            ${mpi_mgmt_startup}[${i}] = 1;
                        };

                        if ( ${name} =~ /^MPI_(Comm|Cart)/ )
                        {
                            ${mpi_mgmt_comm}[${i}] = 1;
                        };

                        //--- File I/O

                        if ( ${name} =~ /^MPI_File/ )
                        {
                            if (
                                not ( ${name} =~ /^MPI_File_set_err/ )
                                and
                                (
                                    ( ${name} =~ /^MPI_File_(open|close|preallocate|seek_shared|sync)$/ )
                                    or
                                    ( ${name} =~ /^MPI_File.*_(all|ordered|set)/ )
                                )
                               )
                            {
                                ${mpi_io_coll}[${i}] = 1;
                            }
                            else
                            {
                                ${mpi_io_noncoll}[${i}] = 1;
                            };
                        };
                    }
                    elseif ( ${paradigm} eq "openmp" )
                    {
                        ${includesOpenMP} = 1;

                        //--- Synchronization

                        if ( ${role} eq "barrier" )
                        {
                            ${omp_sync_ebarrier}[${i}] = 1;
                        };
                        if ( ${role} eq "implicit barrier" )
                        {
                            ${omp_sync_ibarrier}[${i}] = 1;
                        };
                        if ( ${role} =~ /^(atomic|critical)$/ )
                        {
                            ${omp_sync_critical}[${i}] = 1;
                        };
                        if ( ${name} =~ /^omp_(destroy|init|set|test|unset)(_nest){0,1}_lock$/ )
                        {
                            ${omp_sync_lock_api}[${i}] = 1;
                        };
                        if ( ${role} eq "flush" )
                        {
                            ${omp_sync_flush}[${i}] = 1;
                        };
                        if ( ${role} eq "ordered" )
                        {
                            ${omp_sync_ordered}[${i}] = 1;
                        };
                    }
                    elseif ( ${paradigm} eq "pthread" )
                    {
                        ${includesPthread} = 1;

                        //--- Synchronization

                        if ( ${name} =~ /^pthread_mutex_*/ )
                        {
                            ${pthread_sync_mutex_api}[${i}] = 1;
                        };
                        if ( ${name} =~ /^pthread_cond_*/ )
                        {
                            ${pthread_sync_cond_api}[${i}] = 1;
                        };

                        //--- Management

                        if ( ${name} =~ /^pthread_(create|join|exit|abort|cancel|detach)$/ )
                        {
                            ${pthread_mgmt}[${i}] = 1;
                        };
                    }
                    elseif ( ${paradigm} eq "opencl" )
                    {
                        ${includesOpenCL} = 1;

                        //--- Computation

                        if ( ${cube::region::mod}[${regionid}] eq "OPENCL_KERNEL" )
                        {
                            ${ocl_comp}[${i}] = 1;
                        };

                        //--- Synchronization

                        if ( ${name} =~ /^(clFinish|clWaitForEvents|clEnqueueWaitForEvents|clEnqueueBarrier|clEnqueueMarker|clEnqueueMarkerWithWaitList|clEnqueueBarrierWithWaitList)$/ )
                        {
                            ${ocl_sync}[${i}] = 1;
                        };

                        if ( ${name} eq "WAIT FOR COMMAND QUEUE" )
                        {
                            ${opencl_sync}[${i}] = 1;
                        };

                        //--- Communication

                        if ( ${name} =~ /^cl(CreateBuffer|CreateSubBuffer|EnqueueReadBuffer|EnqueueReadBufferRect|EnqueueWriteBuffer|EnqueueWriteBufferRect|EnqueueFillBuffer|EnqueueCopyBuffer|EnqueueCopyBufferRect|EnqueueMapBuffer|EnqueueUnmapMemObjectCopy|EnqueueMigrateMemObjects|CreatePipe|SVMAlloc|SVMFree|EnqueueSVMFree|EnqueueSVMMemcpy|EnqueueSVMMemFill|EnqueueSVMMap|EnqueueSVMUnmap|CreateImage|CreateImage2D|CreateImage3D|EnqueueReadImage|EnqueueWriteImage|EnqueueCopyImage|EnqueueCopyImageToBuffer|EnqueueCopyBufferToImage|EnqueueMapImage|EnqueueFillImage|CreateSamplerWithProperties|CreateSampler|ReleaseSampler|RetainSampler|RetainMemObject|ReleaseMemObject)$/ )
                        {
                            ${ocl_comm}[${i}] = 1;
                        };

                        //--- Management

                        if ( ${name} =~ /^cl(GetPlatformIDs|GetPlatformInfo|GetDeviceIDs|GetDeviceInfo|CreateSubDevices|RetainDevice|ReleaseDevice|CreateContext|CreateContextFromType|RetainContext|ReleaseContext|GetContextInfo|CreateProgramWithSource|CreateProgramWithBinary|RetainProgram|ReleaseProgram|GetProgramInfo|GetProgramBuildInfo|CreateKernelsInProgram|CreateProgramWithBuildInKernels|BuildProgram|CompileProgram|LinkProgram|CreateKernel|Retainkernel|ReleaseKernel|GetEventInfo|RetainEvent|ReleaseEvent|GetEventProfilingInfo|CreateUserInfo|SetUserEventStatus|SetEventCallback|CreateCommandQueue|CreateCommandQueueWithProperties|RetainCommandQueue|ReleaseCommandQueue|GetCommandQueueInfo|SetCommandQueueProperty|GetKernelInfo|GetKernelWorkGroupInfo|GetKernelArgInfo|Flush|UnloadCompiler|UnloadPlatformCompiler|GetExtensionFunctionAddress|GetExtensionFunctionAddressForPlatform|SetMemObjectDestructorCallback|GetMemObjectInfo|GetPipeInfo|GetSupportedImageFormats|GetImageInfo|GetSamplerInfo)$/ )
                        {
                            ${ocl_mgmt}[${i}] = 1;
                        };

                        if ( ${name} eq "BUFFER FLUSH" )
                        {
                            ${opencl}[${i}] = 0;
                            ${execution}[${i}] = 0;
                            ${overhead}[${i}]  = 1;
                        };
                    }
                    elseif ( ${paradigm} eq "cuda" )
                    {
                        ${includesCUDA} = 1;

                        //--- CUDA Runtime API

                        //--- Management

                        if ( ${name} =~ /^cuda(ChooseDevice|DeviceGetAttribute|DeviceGetByPCIBusId|DeviceGetCacheConfig|DeviceGetLimit|DeviceGetPCIBusId|DeviceGetSharedMemConfig|DeviceGetStreamPriorityRange|DeviceReset|DeviceSetCacheConfig|DeviceSetLimit|DeviceSetSharedMemConfig|GetDevice|GetDeviceCount|GetDeviceFlags|GetDeviceProperties|SetDevice|SetDeviceFlags|SetValidDevices|ThreadExit|ThreadGetCacheConfig|ThreadGetLimit|ThreadSetCacheConfig|ThreadSetLimit|StreamCreate|StreamCreateWithFlags|StreamCreateWithPriority|StreamDestroy|EventCreate|EventCreateWithFlags|EventDestroy|FuncSetCacheConfig)$/ )
                        {
                            ${cuda_mgmt}[${i}] = 1;
                        };

                        //--- Communication

                        if ( ${name} =~ /^cuda(Free|FreeArray|FreeHost|FreeMipmappedArray|HostAlloc|Malloc|Malloc3D|Malloc3DArray|MallocArray|MallocHost|MallocManaged|MallocMipmappedArray|MallocPitch|Memcpy|Memcpy2D|Memcpy2DArrayToArray|Memcpy2DAsync|Memcpy2DFromArray|Memcpy2DFromArrayAsync|Memcpy2DToArray|Memcpy2DToArrayAsync|Memcpy3D|Memcpy3DAsync|MemcpyPeer|MemcpyPeerAsync|MemcpyArrayToArray|MemcpyAsync|MemcpyFromArray|MemcpyFromArrayAsync|MemcpyFromSymbol|MemcpyFromSymbolAsync|MemcpyFromPeer|MemcpyFromPeerAsync|MemcpyToArray|MemcpyToArrayAsync|MemcpyToSymbol|MemcpyToSymbolAsync)$/ )
                        {
                            ${cuda_comm}[${i}] = 1;
                        };

                        //--- Synchronization

                        if ( ${name} =~ /^cuda(StreamWaitEvent|StreamSynchronize|EventSynchronize|DeviceSynchronize)$/ )
                        {
                            ${cuda_sync}[${i}] = 1;
                        };

                        //--- CUDA Driver API

                        //--- Management

                        if ( ${name} =~ /^cu(Init|DeviceGet|DeviceGetAttribute|DeviceGetCount|DeviceGetName|DeviceTotalMem|DeviceComputeCapability|DeviceGetProperties|DevicePrimaryCtxGetState|DevicePrimaryCtxRelease|DevicePrimaryCtxReset|DevicePrimaryCtxRetain|DevicePrimaryCtxSetFlags|CtxCrate|CtxDestroy|CtxGetApiVersion|CtxGetCacheConfig|CtxGetCurrent|CtxGetDevice|CtxGetFlags|CtxGetLimit|CtxGetSharedMemConfig|CtxGetStreamPriorityRange|CtxPopCurrent|CtxPushCurrent|CtxSetCacheConfig|CtxSetCurrent|CtxSetLimit|CtxSetSharedMemConfig|CtxAttach|CtxDetach|LinkCreate|LinkDestroy|EventCreate|EventDestroy|StreamCreate|StreamCreateWithPriority|StreamDestroy|MemFree|MemFreeHost|MemHostAlloc)$/ )
                        {
                            ${cuda_mgmt}[${i}] = 1;
                        };

                        //--- Communication

                        if ( ${name} =~ /^cu(Array3DCreate|ArrayCreate|ArrayDestroy|MemAlloc|MemAllocHost|MemAllocManaged|MemAllocPitch|Memcpy|Memcpy2D|Memcpy2DAsync|Memcpy2DUnaligned|Memcpy3D|Memcpy3DAsync|Memcpy3DPeer|Memcpy3DPeerAsync|MemcpyAsync|MemcpyAtoA|MemcpyAtoD|MemcpyAtoH|MemcpyAtoHAsync|MemcpyDtoA|MemcpyDtoD|MemcpyDtoDAsync|MemcpyDtoH|MemcpyDtoHAsync|MemcpyHtoA|MemcpyHtoAAsync|MemcpyHtoD|MemcpyHtoDAsync|MemcpyPeer|MemcpyPeerAsync|MemsetD16|MemsetD16Async|MemsetD2D16|MemsetD2D16Async|MemsetD2D32|MemsetD2D32Async|MemsetD2D8|MemsetD2D8Async|MemsetD32|MemsetD32Async|MemsetD8|MemsetD8Async|MipmappedArrayCreate|MipmappedArrayDestroy)$/ )
                        {
                            ${cuda_comm}[${i}] = 1;
                        };

                        //--- Synchronization

                        if ( ${name} =~ /^cu(CtxSynchronize|StreamWaitEvent|StreamSynchronize|EventSynchronize)$/ )
                        {
                            ${cuda_sync}[${i}] = 1;
                        };

                        if ( ${name} eq "DEVICE SYNCHRONIZE" )
                        {
                            ${cuda_sync}[${i}] = 1;
                        };

                        //--- Computation

                        if ( ${cube::region::mod}[${regionid}] seq "CUDA_KERNEL" )
                        {
                            ${cuda_comp}[${i}] = 1;
                        };

                        if ( ${name} eq "BUFFER FLUSH" )
                        {
                            ${cuda}[${i}] = 0;
                            ${execution}[${i}] = 0;
                            ${overhead}[${i}]  = 1;
                        };
                    }
                    elseif (
                        ( ${paradigm} eq "measurement" )
                        or
                        ( ${name} eq "BUFFER FLUSH" )
                        or 
                        ( ${name} eq "MEASUREMENT OFF" )
                        or
                        ( ( ${paradigm} eq "thread-fork-join" ) and ( ${name} eq "TASKS" ) )
                       )
                    {
                        ${execution}[${i}] = 0;
                        ${overhead}[${i}]  = 1;
                    };

                    ${i} = ${i} + 1;
                };


                //--- Disable unused metric subtrees ------------------------

                if ( ${includesMPI} == 0 )
                {
                    cube::metric::set::mpi_sync("value", "VOID");
                    cube::metric::set::mpi_comm("value", "VOID");
                    cube::metric::set::mpi_mgmt("value", "VOID");
                    cube::metric::set::mpi_io("value", "VOID");
                };
                if ( ${includesOpenMP} == 0 )
                {
                    cube::metric::set::omp_sync("value", "VOID");
                };
                if ( ${includesPthread} == 0 )
                {
                    cube::metric::set::pthread_sync("value", "VOID");
                    cube::metric::set::pthread_mgmt("value", "VOID");
                };
                if ( ${includesOpenCL} == 0 )
                {
                    cube::metric::set::ocl_comp("value", "VOID");
                    cube::metric::set::ocl_sync("value", "VOID");
                    cube::metric::set::ocl_comm("value", "VOID");
                    cube::metric::set::ocl_mgmt("value", "VOID");
                };
                if ( ${includesCUDA} == 0 )
                {
                    cube::metric::set::cuda_comp("value", "VOID");
                    cube::metric::set::cuda_sync("value", "VOID");
                    cube::metric::set::cuda_comm("value", "VOID");
                    cube::metric::set::cuda_mgmt("value", "VOID");
                };
                return 0;
            }
            </cubeplinit>
            <metric type="POSTDERIVED">
                <disp_name>Computation</disp_name>
                <uniq_name>comp</uniq_name>
                <dtype>FLOAT</dtype>
                <uom>sec</uom>
                <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#comp</url>
                <descr>Total computation time</descr>
                <cubepl>
                    metric::execution() - metric::sync() - metric::comm() - metric::mgmt() - metric::io()
                </cubepl>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>OpenCL kernels</disp_name>
                    <uniq_name>ocl_comp</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#ocl_comp</url>
                    <descr>Time spent during execution of OpenCL kernels</descr>
                    <cubepl>
                        ${ocl_comp}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>CUDA kernels</disp_name>
                    <uniq_name>cuda_comp</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#cuda_comp</url>
                    <descr>Time spent during execution of CUDA kernels</descr>
                    <cubepl>
                        ${cuda_comp}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
            </metric>
            <metric type="POSTDERIVED">
                <disp_name>Synchronization</disp_name>
                <uniq_name>sync</uniq_name>
                <dtype>FLOAT</dtype>
                <uom>sec</uom>
                <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#sync</url>
                <descr>Total time spent on syncronization</descr>
                <cubepl>
                    metric::mpi_sync() + metric::omp_sync() + metric::pthread_sync() + metric::ocl_sync() + metric::cuda_sync()
                </cubepl>
                <metric type="POSTDERIVED">
                    <disp_name>MPI</disp_name>
                    <uniq_name>mpi_sync</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_sync</url>
                    <descr>Time spent on MPI synchronization</descr>
                    <cubepl>
                        metric::mpi_sync_barrier() + metric::mpi_sync_active_target() + metric::mpi_sync_passive_target()
                    </cubepl>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Barrier</disp_name>
                        <uniq_name>mpi_sync_barrier</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_sync_barrier</url>
                        <descr>Time spent on MPI barriers</descr>
                        <cubepl>
                            ${mpi_sync_barrier}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Active target</disp_name>
                        <uniq_name>mpi_sync_active_target</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_sync_active_target</url>
                        <descr>Time spent on active target synchronization</descr>
                        <cubepl>
                            ${mpi_sync_active_target}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Passive target</disp_name>
                        <uniq_name>mpi_sync_passive_target</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_sync_passive_target</url>
                        <descr>Time spent on passive target synchronization</descr>
                        <cubepl>
                            ${mpi_sync_passive_target}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
                <metric type="POSTDERIVED">
                    <disp_name>OpenMP</disp_name>
                    <uniq_name>omp_sync</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync</url>
                    <descr>Time spent on OpenMP synchronization</descr>
                    <cubepl>
                        metric::omp_sync_barrier() + metric::omp_sync_critical() + metric::omp_sync_lock_api() + metric::omp_sync_flush() + metric::omp_sync_ordered()
                    </cubepl>
                    <metric type="POSTDERIVED">
                        <disp_name>Barrier</disp_name>
                        <uniq_name>omp_sync_barrier</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_barrier</url>
                        <descr>Time spent on OpenMP barriers</descr>
                        <cubepl>
                            metric::omp_sync_ebarrier() + metric::omp_sync_ibarrier()
                        </cubepl>
                        <metric type="PREDERIVED_EXCLUSIVE">
                            <disp_name>Explicit</disp_name>
                            <uniq_name>omp_sync_ebarrier</uniq_name>
                            <dtype>FLOAT</dtype>
                            <uom>sec</uom>
                            <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_ebarrier</url>
                            <descr>Time spent on explicit OpenMP barriers</descr>
                            <cubepl>
                                ${omp_sync_ebarrier}[${calculation::callpath::id}] * metric::time(e)
                            </cubepl>
                        </metric>
                        <metric type="PREDERIVED_EXCLUSIVE">
                            <disp_name>Implicit</disp_name>
                            <uniq_name>omp_sync_ibarrier</uniq_name>
                            <dtype>FLOAT</dtype>
                            <uom>sec</uom>
                            <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_ibarrier</url>
                            <descr>Time spent on implicit OpenMP barriers</descr>
                            <cubepl>
                                ${omp_sync_ibarrier}[${calculation::callpath::id}] * metric::time(e)
                            </cubepl>
                        </metric>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Critical & atomic</disp_name>
                        <uniq_name>omp_sync_critical</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_critical</url>
                        <descr>Time spent in front of a critical section</descr>
                        <cubepl>
                            ${omp_sync_critical}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Lock API</disp_name>
                        <uniq_name>omp_sync_lock_api</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_lock_api</url>
                        <descr>Time spent in OpenMP API calls dealing with locks</descr>
                        <cubepl>
                            ${omp_sync_lock_api}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Flush</disp_name>
                        <uniq_name>omp_sync_flush</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_flush</url>
                        <descr>Time spent in the OpenMP flush directives</descr>
                        <cubepl>
                            ${omp_sync_flush}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Ordered</disp_name>
                        <uniq_name>omp_sync_ordered</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#omp_sync_ordered</url>
                        <descr>Time spent in front of an ordered region</descr>
                        <cubepl>
                            ${omp_sync_ordered}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
                <metric type="POSTDERIVED">
                    <disp_name>POSIX thread</disp_name>
                    <uniq_name>pthread_sync</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#pthread_sync</url>
                    <descr>Time spent on Pthreads synchronization</descr>
                    <cubepl>
                        metric::pthread_sync_mutex_api() + metric::pthread_sync_cond_api()
                    </cubepl>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Mutex API</disp_name>
                        <uniq_name>pthread_sync_mutex_api</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#pthread_sync_mutex_api</url>
                        <descr>Time spent in Pthread mutexes</descr>
                        <cubepl>
                            ${pthread_sync_mutex_api}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Condition API</disp_name>
                        <uniq_name>pthread_sync_cond_api</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#pthread_sync_cond_api</url>
                        <descr>Time spent in Pthread conditional syncronization routines</descr>
                        <cubepl>
                            ${pthread_cond_mutex_api}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>OpenCL</disp_name>
                    <uniq_name>ocl_sync</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#ocl_sync</url>
                    <descr>Time spent on OpenCL synchronization</descr>
                    <cubepl>
                        ${ocl_sync}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>CUDA</disp_name>
                    <uniq_name>cuda_sync</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#cuda_sync</url>
                    <descr>Time spent on CUDA synchronization</descr>
                    <cubepl>
                        ${cuda_sync}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
            </metric>
            <metric type="POSTDERIVED">
                <disp_name>Communication</disp_name>
                <uniq_name>comm</uniq_name>
                <dtype>FLOAT</dtype>
                <uom>sec</uom>
                <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#comm</url>
                <descr>Total time spent on communication</descr>
                <cubepl>
                    metric::mpi_comm() + metric::ocl_comm() + metric::cuda_comm()
                </cubepl>
                <metric type="POSTDERIVED">
                    <disp_name>MPI</disp_name>
                    <uniq_name>mpi_comm</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_comm</url>
                    <descr>Time spent on MPI communication</descr>
                    <cubepl>
                        metric::mpi_comm_p2p() + metric::mpi_comm_coll() + metric::mpi_comm_1sided()
                    </cubepl>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Point-to-point</disp_name>
                        <uniq_name>mpi_comm_p2p</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_comm_p2p</url>
                        <descr>Time spent on MPI point-to-point operations</descr>
                        <cubepl>
                            ${mpi_comm_p2p}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Collective</disp_name>
                        <uniq_name>mpi_comm_coll</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_comm_coll</url>
                        <descr>Time spent on MPI collective operations</descr>
                        <cubepl>
                            ${mpi_comm_coll}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>One-sided</disp_name>
                        <uniq_name>mpi_comm_1sided</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_comm_1sided</url>
                        <descr>Time spent on MPI one-sided communication</descr>
                        <cubepl>
                            ${mpi_comm_1sided}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>OpenCL</disp_name>
                    <uniq_name>ocl_comm</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#ocl_comm</url>
                    <descr>Time spent on OpenCL communication</descr>
                    <cubepl>
                        ${ocl_comm}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>CUDA</disp_name>
                    <uniq_name>cuda_comm</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#cuda_comm</url>
                    <descr>Time spent on CUDA communication</descr>
                    <cubepl>
                        ${cuda_comm}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
            </metric>
            <metric type="POSTDERIVED">
                <disp_name>Management</disp_name>
                <uniq_name>mgmt</uniq_name>
                <dtype>FLOAT</dtype>
                <uom>sec</uom>
                <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mgmt</url>
                <descr>Total time spent on management</descr>
                <cubepl>
                    metric::mpi_mgmt() + metric::pthread_mgmt() + metric::ocl_mgmt() + metric::cuda_mgmt()
                </cubepl>
                <metric type="POSTDERIVED">
                    <disp_name>MPI</disp_name>
                    <uniq_name>mpi_mgmt</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_mgmt</url>
                    <descr>Time spent on management of MPI</descr>
                    <cubepl>
                        metric::mpi_mgmt_startup() + metric::mpi_mgmt_comm()
                    </cubepl>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Startup</disp_name>
                        <uniq_name>mpi_mgmt_startup</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_mgmt_startup</url>
                        <descr>Time needed to initialize and finalize MPI</descr>
                        <cubepl>
                            ${mpi_mgmt_startup}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Communicator</disp_name>
                        <uniq_name>mpi_mgmt_comm</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_mgmt_comm</url>
                        <descr>Time needed for management of MPI communicators</descr>
                        <cubepl>
                            ${mpi_mgmt_comm}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>POSIX thread</disp_name>
                    <uniq_name>pthread_mgmt</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#pthread_mgmt</url>
                    <descr>Time spent on management of Pthreads</descr>
                    <cubepl>
                        ${pthread_mgmt}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>OpenCL</disp_name>
                    <uniq_name>ocl_mgmt</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#ocl_mgmt</url>
                    <descr>Time spent on management of OpenCL</descr>
                    <cubepl>
                        ${ocl_mgmt}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
                <metric type="PREDERIVED_EXCLUSIVE">
                    <disp_name>CUDA</disp_name>
                    <uniq_name>cuda_mgmt</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#cuda_mgmt</url>
                    <descr>Time spent on management of CUDA</descr>
                    <cubepl>
                        ${cuda_mgmt}[${calculation::callpath::id}] * metric::time(e)
                    </cubepl>
                </metric>
            </metric>
            <metric type="POSTDERIVED">
                <disp_name>File I/O</disp_name>
                <uniq_name>io</uniq_name>
                <dtype>FLOAT</dtype>
                <uom>sec</uom>
                <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#io</url>
                <descr>Total time spent on File I/O</descr>
                <cubepl>
                    metric::mpi_io()
                </cubepl>
                <metric type="POSTDERIVED">
                    <disp_name>MPI</disp_name>
                    <uniq_name>mpi_io</uniq_name>
                    <dtype>FLOAT</dtype>
                    <uom>sec</uom>
                    <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_io</url>
                    <descr>Time spent in MPI I/O operations</descr>
                    <cubepl>
                        metric::mpi_io_noncoll() + metric::mpi_io_coll()
                    </cubepl>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Non-collective</disp_name>
                        <uniq_name>mpi_io_noncoll</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_io_noncoll</url>
                        <descr>Time spent in MPI non-collective I/O operations</descr>
                        <cubepl>
                            ${mpi_io_noncoll}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                    <metric type="PREDERIVED_EXCLUSIVE">
                        <disp_name>Collective</disp_name>
                        <uniq_name>mpi_io_coll</uniq_name>
                        <dtype>FLOAT</dtype>
                        <uom>sec</uom>
                        <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#mpi_io_coll</url>
                        <descr>Time spent in MPI collective I/O operations</descr>
                        <cubepl>
                            ${mpi_io_coll}[${calculation::callpath::id}] * metric::time(e)
                        </cubepl>
                    </metric>
                </metric>
            </metric>
        </metric>
        <metric type="PREDERIVED_EXCLUSIVE">
            <disp_name>Overhead</disp_name>
            <uniq_name>overhead</uniq_name>
            <dtype>FLOAT</dtype>
            <uom>sec</uom>
            <url>@mirror@scorep_metrics-@PACKAGE_VERSION@.html#overhead</url>
            <descr>Overhead</descr>
            <cubepl>
                ${overhead}[${calculation::callpath::id}] * metric::time(e)
            </cubepl>
    </metric>
</metrics>
