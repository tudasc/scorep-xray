/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Description of the substrate plugin events header.
 *        For information on how to use substrate plugins, please refer to section @secref{substrate_plugins}.
 */
#ifndef SCOREP_SUBSTRATE_EVENTS_H
#define SCOREP_SUBSTRATE_EVENTS_H

/**
 * @section advice_substrate_events Advice
 * Do not include this file directly, but include SCOREP_SubstratePlugins.h
 *
 */

#include <stdbool.h>
#include <stddef.h>

#include <scorep/SCOREP_MetricTypes.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <scorep/SCOREP_PublicHandles.h>


/**
 * Generic void function pointer for substrate functions.
 */
typedef void ( * SCOREP_Substrates_Callback )( void );

struct SCOREP_Location;

/**
 * Substrates need to provide two sets of callbacks for the modes
 * SCOREP_SUBSTATES_RECORDING_ENABLED and SCOREP_SUBSTRATES_RECORDING_DISABLED.
 * This enum is used as an array index.
 */
typedef enum SCOREP_Substrates_Mode
{
    SCOREP_SUBSTRATES_RECORDING_ENABLED = 0, /**< The recording of events is enabled (default) */
    SCOREP_SUBSTRATES_RECORDING_DISABLED,    /**< The recording of events is disabled */

    SCOREP_SUBSTRATES_NUM_MODES              /**< Non-ABI */
} SCOREP_Substrates_Mode;

/**
 * \enum SCOREP_Substrates_EventType
 * \brief Substrate events. Lists every event that is going to be used by the substrate mechanism. More details can be found in the respective functions.
 * To maintain API stability, new events need to be added at the end of the enum.
 */
typedef enum SCOREP_Substrates_EventType
{
    SCOREP_EVENT_ENABLE_RECORDING = 0,            /**< enable recording of events, see SCOREP_Substrates_EnableRecordingCb() */
    SCOREP_EVENT_DISABLE_RECORDING,               /**< disable recording of events, see SCOREP_Substrates_DisableRecordingCb() */
    SCOREP_EVENT_ON_TRACING_BUFFER_FLUSH_BEGIN,   /**< start flushing trace buffer to disk, see SCOREP_Substrates_OnTracingBufferFlushBeginCb() */
    SCOREP_EVENT_ON_TRACING_BUFFER_FLUSH_END,     /**< end flushing trace buffer to disk, see SCOREP_Substrates_OnTracingBufferFlushEndCb() */
    SCOREP_EVENT_ENTER_REGION,                    /**< enter an instrumented region, see SCOREP_Substrates_EnterRegionCb() */
    SCOREP_EVENT_EXIT_REGION,                     /**< exit an instrumented region, see SCOREP_Substrates_ExitRegionCb() */
    SCOREP_EVENT_SAMPLE,                          /**< record a calling context from sampling, see SCOREP_Substrates_SampleCb() */
    SCOREP_EVENT_CALLING_CONTEXT_ENTER,           /**< enter an instrumented region with calling context information, replaces SCOREP_EVENT_ENTER_REGION when unwinding is enabled, see SCOREP_Substrates_CallingContextEnterCb() */
    SCOREP_EVENT_CALLING_CONTEXT_EXIT,            /**< exit an instrumented region with calling context information, replaces SCOREP_EVENT_EXIT_REGION when unwinding is enabled, see SCOREP_Substrates_CallingContextExitCb() */
    SCOREP_EVENT_ENTER_REWIND_REGION,             /**< enter rewinding, see SCOREP_Substrates_EnterRewindRegionCb() */
    SCOREP_EVENT_EXIT_REWIND_REGION,              /**< exit rewinding, see SCOREP_Substrates_ExitRewindRegionCb() */
    SCOREP_EVENT_MPI_SEND,                        /**< MPI_Send, see SCOREP_Substrates_MpiSendCb() */
    SCOREP_EVENT_MPI_RECV,                        /**< MPI_Recv, see SCOREP_Substrates_MpiRecvCb() */
    SCOREP_EVENT_MPI_COLLECTIVE_BEGIN,            /**< starts an MPI collective, see SCOREP_Substrates_MpiCollectiveBeginCb() */
    SCOREP_EVENT_MPI_COLLECTIVE_END,              /**< ends an MPI collective, see SCOREP_Substrates_MpiCollectiveEndCb() */
    SCOREP_EVENT_MPI_ISEND_COMPLETE,              /**< marks the completion of an MPI_Isend, see SCOREP_Substrates_MpiIsendCompleteCb() */
    SCOREP_EVENT_MPI_IRECV_REQUEST,               /**< marks the request for an MPI_Irecv, see SCOREP_Substrates_MpiIrecvRequestCb() */
    SCOREP_EVENT_MPI_REQUEST_TESTED,              /**< marks the test of an MPI request (e.g., in an MPI_Waitsome(...) ), see SCOREP_Substrates_MpiRequestTestedCb() */
    SCOREP_EVENT_MPI_REQUEST_CANCELLED,           /**< marks the cancellation of an MPI request (e.g., an MPI_Test_cancelled(...) call that returned true in its second parameter), see SCOREP_Substrates_MpiRequestCancelledCb() */
    SCOREP_EVENT_MPI_ISEND,                       /**< marks the start of an MPI_ISend, see SCOREP_Substrates_MpiIsendCb() */
    SCOREP_EVENT_MPI_IRECV,                       /**< marks the start of an MPI_IRecv, see SCOREP_Substrates_MpiIrecvCb() */
    SCOREP_EVENT_RMA_WIN_CREATE,                  /**< marks the creation of an RMA window (used by cuda, opencl, and shmem), see SCOREP_Substrates_RmaWinCreateCb() */
    SCOREP_EVENT_RMA_WIN_DESTROY,                 /**< marks the destruction of an RMA window (used by cuda, opencl, and shmem), see SCOREP_Substrates_RmaWinDestroyCb() */
    SCOREP_EVENT_RMA_COLLECTIVE_BEGIN,            /**< marks the start of an RMA collective (used by shmem), see SCOREP_Substrates_RmaCollectiveBeginCb() */
    SCOREP_EVENT_RMA_COLLECTIVE_END,              /**< marks the start of an RMA collective (used by shmem), see SCOREP_Substrates_RmaCollectiveEndCb() */
    SCOREP_EVENT_RMA_TRY_LOCK,                    /**< marks an RMA trylock (used by shmem), see SCOREP_Substrates_RmaTryLockCb() */
    SCOREP_EVENT_RMA_ACQUIRE_LOCK,                /**< marks the acquisition of an RMA lock (used by shmem), see SCOREP_Substrates_RmaAcquireLockCb() */
    SCOREP_EVENT_RMA_REQUEST_LOCK,                /**< marks a request for an RMA lock (used by shmem), see SCOREP_Substrates_RmaRequestLockCb() */
    SCOREP_EVENT_RMA_RELEASE_LOCK,                /**< marks a release of an RMA lock (used by shmem), see SCOREP_Substrates_RmaReleaseLockCb() */
    SCOREP_EVENT_RMA_SYNC,                        /**< marks a simple pairwise RMA synchronization, see SCOREP_Substrates_RmaSyncCb() */
    SCOREP_EVENT_RMA_GROUP_SYNC,                  /**< marks an RMA synchronization of a sub-group of locations on a given window, see SCOREP_Substrates_RmaGroupSyncCb() */
    SCOREP_EVENT_RMA_PUT,                         /**< marks a put operation to an RMA memory (used by cuda, opencl, and shmem), see SCOREP_Substrates_RmaPutCb() */
    SCOREP_EVENT_RMA_GET,                         /**< marks a get operation from an RMA memory (used by cuda, opencl, and shmem), see SCOREP_Substrates_RmaGetCb() */
    SCOREP_EVENT_RMA_ATOMIC,                      /**< marks an atomic RMA operation (used by shmem), see SCOREP_Substrates_RmaAtomicCb() */
    SCOREP_EVENT_RMA_WAIT_CHANGE,                 /**< marks a blocks until a remote operation modifies a given RMA memory field (used by shmem), see SCOREP_Substrates_RmaWaitChangeCb() */
    SCOREP_EVENT_RMA_OP_COMPLETE_BLOCKING,        /**< marks completion of a blocking RMA operation (used by cuda, opencl, and shmem), see SCOREP_Substrates_RmaOpCompleteBlockingCb() */
    SCOREP_EVENT_RMA_OP_COMPLETE_NON_BLOCKING,    /**< marks completion of a non-blocking RMA operation, see SCOREP_Substrates_RmaOpCompleteNonBlockingCb() */
    SCOREP_EVENT_RMA_OP_TEST,                     /**< marks a test for completion of a non-blocking RMA operation, see SCOREP_Substrates_RmaOpTestCb() */
    SCOREP_EVENT_RMA_OP_COMPLETE_REMOTE,          /**< marks a remote completion point, see SCOREP_Substrates_RmaOpCompleteRemoteCb() */
    SCOREP_EVENT_THREAD_ACQUIRE_LOCK,             /**< marks when a thread acquires a lock (pthreads, explicit and implicit OpenMP locks), see SCOREP_Substrates_ThreadAcquireLockCb() */
    SCOREP_EVENT_THREAD_RELEASE_LOCK,             /**< marks when a thread releases a lock (pthreads, explicit and implicit OpenMP locks), see SCOREP_Substrates_ThreadreleaseLockCb() */
    SCOREP_EVENT_TRIGGER_COUNTER_INT64,           /**< called when an int64 counter is triggered, see SCOREP_Substrates_TriggerCounterInt64Cb() */
    SCOREP_EVENT_TRIGGER_COUNTER_UINT64,          /**< called when an uint64 counter is triggered, see SCOREP_Substrates_TriggerCounterUint64Cb() */
    SCOREP_EVENT_TRIGGER_COUNTER_DOUBLE,          /**< called when an double counter is triggered, see SCOREP_Substrates_TriggerCounterDoubleCb() */
    SCOREP_EVENT_TRIGGER_PARAMETER_INT64,         /**< called when an int64 parameter is triggered, called from user instrumentation, see SCOREP_Substrates_TriggerParameterInt64Cb() */
    SCOREP_EVENT_TRIGGER_PARAMETER_UINT64,        /**< called when an uint64 parameter is triggered, called from user instrumentation, see SCOREP_Substrates_TriggerParameterUint64Cb() */
    SCOREP_EVENT_TRIGGER_PARAMETER_STRING,        /**< called when an string parameter is triggered, called from user instrumentation, see SCOREP_Substrates_TriggerParameterStringCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_FORK,           /**< called before a fork-join based thread-parallel programming model (e.g., OpenMP) forks its threads logically, see SCOREP_Substrates_ThreadForkJoinForkCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_JOIN,           /**< called after a fork-join based thread-parallel programming model (e.g., OpenMP) joins its threads logically, see SCOREP_Substrates_ThreadForkJoinJoinCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_BEGIN,     /**< begin of a parallel execution on a thread created by either SCOREP_ThreadForkJoin_Fork, is called by all created threads, see SCOREP_Substrates_ThreadForkJoinTeamBeginCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_END,       /**< end of a parallel execution on a thread created by either SCOREP_ThreadForkJoin_Fork, is called by all created threads, see SCOREP_Substrates_ThreadForkJoinTeamBeginCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_CREATE,    /**< creation of a task in a fork-join based thread-parallel programming model (e.g., OpenMP), see SCOREP_Substrates_ThreadForkJoinTaskCreateCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_SWITCH,    /**< switching of tasks in a fork-join based thread-parallel programming model (e.g., OpenMP), see SCOREP_Substrates_ThreadForkJoinTaskSwitchCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_BEGIN,     /**< begin of a task in a fork-join based thread-parallel programming model (e.g., OpenMP), see SCOREP_Substrates_ThreadForkJoinTaskBeginCb() */
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_END,       /**< end of a task in a fork-join based thread-parallel programming model (e.g., OpenMP), see SCOREP_Substrates_ThreadForkJoinTaskEndCb() */
    SCOREP_EVENT_THREAD_CREATE_WAIT_CREATE,       /**< create a new thread in a create-wait based thread-parallel programming model (e.g., pthreads), called by parent, see SCOREP_Substrates_ThreadCreateWaitCreateCb() */
    SCOREP_EVENT_THREAD_CREATE_WAIT_WAIT,         /**< wait and join a thread in a create-wait based thread-parallel programming model (e.g., pthreads), usually called by parent, see SCOREP_Substrates_ThreadCreateWaitWaitCb() */
    SCOREP_EVENT_THREAD_CREATE_WAIT_BEGIN,        /**< begin a new thread in a create-wait based thread-parallel programming model (e.g., pthreads), called by new thread, see SCOREP_Substrates_ThreadCreateWaitBeginCb() */
    SCOREP_EVENT_THREAD_CREATE_WAIT_END,          /**< end a thread in a create-wait based thread-parallel programming model (e.g., pthreads), see SCOREP_Substrates_ThreadCreateWaitEndCb() */
    SCOREP_EVENT_TRACK_ALLOC,                     /**< track malloc/calloc memory allocation, see SCOREP_Substrates_TrackAllocCb() */
    SCOREP_EVENT_TRACK_REALLOC,                   /**< track realloc memory (de-)allocation, see SCOREP_Substrates_TrackReallocCb() */
    SCOREP_EVENT_TRACK_FREE,                      /**< track realloc memory deallocation, see SCOREP_Substrates_TrackFreeCb() */
    SCOREP_EVENT_WRITE_POST_MORTEM_METRICS,       /**< write post mortem metrics before unify, see SCOREP_Substrates_WriteMetricsCb() */
    SCOREP_EVENT_PROGRAM_BEGIN,                   /**< begin of program and measurement, see SCOREP_Substrates_ProgramBeginCb() */
    SCOREP_EVENT_PROGRAM_END,                     /**< end of program and measurement, see SCOREP_Substrates_ProgramEndCb() */
    SCOREP_EVENT_IO_CREATE_HANDLE,                /**< marks the creation of a new I/O handle, the handle gets active and can be used in subsequent I/O operations, see SCOREP_Substrates_IoCreateHandleCb */
    SCOREP_EVENT_IO_DESTROY_HANDLE,               /**< marks the deletion of a, the I/O handle can't be used in subsequent I/O operations any longer, see SCOREP_Substrates_IoDestroyHandleCb */
    SCOREP_EVENT_IO_DUPLICATE_HANDLE,             /**< marks the duplication of an already existing I/O handle, the new one gets active and can be used in subsequent I/O operations, see SCOREP_Substrates_IoDuplicateHandleCb */
    SCOREP_EVENT_IO_SEEK,                         /**< marks a seek operation, sets  the  file  position indicator, see SCOREP_Substrates_IoSeekCb */
    SCOREP_EVENT_IO_CHANGE_STATUS_FLAGS,          /**< called when status information of an I/O handle are changed, see SCOREP_Substrates_IoChangeStatusFlagsCb */
    SCOREP_EVENT_IO_DELETE_FILE,                  /**< marks the deletion of a file, see SCOREP_Substrates_IoDeleteFileCb */
    SCOREP_EVENT_IO_OPERATION_BEGIN,              /**< marks the begin of an I/O operation, see SCOREP_Substrates_IoOperationBeginCb */
    SCOREP_EVENT_IO_OPERATION_ISSUED,             /**< called when an asynchronous I/O operation is submitted to the I/O subssystem, see SCOREP_Substrates_IoOperationIssuedCb */
    SCOREP_EVENT_IO_OPERATION_TEST,               /**< called when an asynchronous I/O operation is tested for completion but is not finished yet, see SCOREP_Substrates_IoOperationTestCb */
    SCOREP_EVENT_IO_OPERATION_COMPLETE,           /**< called when an asynchronous I/O operation is successfully tested for completion, see SCOREP_Substrates_IoOperationCompleteCb */
    SCOREP_EVENT_IO_OPERATION_CANCELLED,          /**< called when an asynchronous I/O operation is aborted, see SCOREP_Substrates_IoOperationCancelledCb */
    SCOREP_EVENT_IO_ACQUIRE_LOCK,                 /**< marks the acquisition of an I/O lock, see SCOREP_Substrates_IoAcquireLockCb */
    SCOREP_EVENT_IO_RELEASE_LOCK,                 /**< marks the release of an I/O lock, see SCOREP_Substrates_IoReleaseLockCb */
    SCOREP_EVENT_IO_TRY_LOCK,                     /**< called when , see SCOREP_Substrates_IoTryLockCb */

    SCOREP_SUBSTRATES_NUM_EVENTS                  /**< Non-ABI, marks the end of the currently supported events and can change with different versions of Score-P (increases with increasing Score-P version) */
} SCOREP_Substrates_EventType;


/**
 * called when enabling the recording on a process and its sub-locations
 * This is triggered by user instrumentation and can not be called when in parallel
 * The default mode is enabled.
 * If the default mode is set to disabled by user instrumentation, an
 * enabled event is called before the substrate is finalized.
 * This event will be generated for SCOREP_Substrates_Mode ENABLED, right after the enabling finished.
 * It is currently not called for SCOREP_Substrates_Mode DISABLED. This might change in the future.
 *
 * @param location location which creates this event.
 *
 * @param timestamp timestamp for this event
 *
 * @param regionHandle "MEASUREMENT OFF" region
 *
 * @param metricValues synchronous metrics at timestamp
 */
typedef void ( * SCOREP_Substrates_EnableRecordingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

/**
 * called when disabling the recording on a process and its sub-locations
 * This is triggered by user instrumentation and can not be called when in parallel
 * This event will be generated for SCOREP_Substrates_Mode ENABLED, right before the disabling starts.
 * It is currently not called for SCOREP_Substrates_Mode DISABLED. This might change in the future.
 *
 * @see SCOREP_Substrates_EnableRecordingCb
 */
typedef void ( * SCOREP_Substrates_DisableRecordingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

/**
 * called when flushing the tracing buffer of a location
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param regionHandle "TRACE BUFFER FLUSH" region
 *
 * @param metricValues synchronous metrics at timestamp
 */
typedef void ( * SCOREP_Substrates_OnTracingBufferFlushBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

/**
 * called after flushing the tracing buffer of a location,
 * @see SCOREP_Substrates_OnTracingBufferFlushBeginCb
 */
typedef void ( * SCOREP_Substrates_OnTracingBufferFlushEndCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_ProgramBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_StringHandle     programName,
    uint32_t                numberOfProgramArgs,
    SCOREP_StringHandle*    programArguments,
    SCOREP_RegionHandle     programRegionHandle );

typedef void ( * SCOREP_Substrates_ProgramEndCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ExitStatus       exitStatus,
    SCOREP_RegionHandle     programRegionHandle );

/**
 * called when entering a region via some instrumentation adapter.
 * If unwinding is enabled, the event CALLING_CONTEXT_ENTER will be called instead.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param regionHandle region that is entered
 *
 * @param metricValues synchronous metrics at timestamp.
 * The synchronous metric belong to the last sampling set definition whose metric occurrence is SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT and whose class is SCOREP_SAMPLING_SET_CPU
 */
typedef void ( * SCOREP_Substrates_EnterRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

/**
 * called when exiting a region via some instrumentation adapter
 * If unwinding is enabled, the event CALLING_CONTEXT_EXIT will be called instead.
 * @see SCOREP_Substrates_EnterRegionCb
 */
typedef void ( * SCOREP_Substrates_ExitRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

/**
 * called when a sampling adapter interrupts the workload and records a sample.
 * Called from a signal handler, so used functions should be async-signal safe.
 * If a function is not signal safe, but is interrupted by a signal
 * (i.e., a sample event) and used within the signal context, its behavior is
 * unpredictable.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param callingContext callstack at timestamp
 *
 * @param previousCallingContext calling context of the last SCOREP_Substrates_SampleCb
 *
 * @param unwindDistance number of stack levels changed since the last sample
 *
 * @param interruptGeneratorHandle source that interrupted the workload
 *
 * @param metricValues synchronous metrics at timestamp
 * The synchronous metric belong to the last sampling set definition whose metric occurrence is SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT and whose class is SCOREP_SAMPLING_SET_CPU
 */
typedef void ( * SCOREP_Substrates_SampleCb )(
    struct SCOREP_Location*         location,
    uint64_t                        timestamp,
    SCOREP_CallingContextHandle     callingContext,
    SCOREP_CallingContextHandle     previousCallingContext,
    uint32_t                        unwindDistance,
    SCOREP_InterruptGeneratorHandle interruptGeneratorHandle,
    uint64_t*                       metricValues );

/**
 * called when entering a region via a instrumentation adapter
 * and Score-P is recording calling contexts (i.e., unwinding is enabled),
 * alternative to SCOREP_Substrates_EnterRegionCb
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param callingContext callstack at timestamp
 *
 * @param previousCallingContext calling context of the last SCOREP_Substrates_SampleCb
 *
 * @param unwindDistance number of stack levels changed since the last sample
 *
 * @param metricValues synchronous metrics at timestamp
 * The synchronous metric belong to the last sampling set definition whose metric occurrence is SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT and whose class is SCOREP_SAMPLING_SET_CPU
 */
typedef void ( * SCOREP_Substrates_CallingContextEnterCb )(
    struct SCOREP_Location*     location,
    uint64_t                    timestamp,
    SCOREP_CallingContextHandle callingContext,
    SCOREP_CallingContextHandle previousCallingContext,
    uint32_t                    unwindDistance,
    uint64_t*                   metricValues );

/**
 * called when exiting a region via a instrumentation adapter
 * and Score-P is recording calling contexts (i.e., unwinding is enabled).
 * alternative to SCOREP_Substrates_ExitRegionCb
 * @see SCOREP_Substrates_CallingContextEnterCb
 */
typedef void ( * SCOREP_Substrates_CallingContextExitCb )(
    struct SCOREP_Location*     location,
    uint64_t                    timestamp,
    SCOREP_CallingContextHandle callingContext,
    SCOREP_CallingContextHandle previousCallingContext,
    uint32_t                    unwindDistance,
    uint64_t*                   metricValues );

/**
 * called when the user adapter enters a rewind region.
 * The recording of the region and any following region information after this
 * should be discarded when the next SCOREP_Substrates_ExitRewindRegionCb for this
 * regionHandle is called with do_rewind == true
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param regionHandle region that is entered
 */
typedef void ( * SCOREP_Substrates_EnterRewindRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle );

/**
 * called when the user adapter exits a rewind region.
 * The recording of the region and any previous regions since
 * SCOREP_Substrates_ExitRewindRegionCb for this regionHandle
 * should be discarded if do_rewind == true
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param regionHandle region that is entered
 *
 * @param doRewind whether to discard previously recorded data or not
 */
typedef void ( * SCOREP_Substrates_ExitRewindRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    bool                    doRewind );

/**
 * called when an MPI_Send is recognized by the MPI adapter
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * There should be a SCOREP_Substrates_MpiRecvCb call on the location of the
 * destinationRank.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param destinationRank rank that should receive the message
 *
 * @param communicatorHandle communicator handle to which the location rank and the destinationRank belong
 *
 * @param tag provided MPI tag for this message
 *
 * @param bytesSent number of bytes sent with this message
 */
typedef void ( * SCOREP_Substrates_MpiSendCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   destinationRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesSent );

/**
 * called when an MPI_Recv is recognized by the MPI adapter
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * There should be a SCOREP_Substrates_MpiSendCb call on the location of the
 * sourceRank.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param sourceRank rank that sent the message that is to be received
 *
 * @param communicatorHandle communicator handle to which the location rank and the destinationRank belong
 *
 * @param tag provided MPI tag for this message
 *
 * @param bytesReceived number of bytes received with this message
 */
typedef void ( * SCOREP_Substrates_MpiRecvCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   sourceRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesReceived );

/**
 * Called when an MPI collective is recognized by the MPI adapter before it is started.
 * See also the MPI specifications at https://www.mpi-forum.org/docs/
 * More information on the type is passed with the SCOREP_Substrates_MpiCollectiveEndCb callback
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 */
typedef void ( * SCOREP_Substrates_MpiCollectiveBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp );


/**
 * called when an MPI collective is recognized by the MPI adapter after it is finished.
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param communicatorHandle communicator handle to which the location rank and the destinationRank belong
 *
 * @param rootRank rank that sent the message that is to be received
 *
 * @param tag provided MPI tag for this message
 *
 * @param bytesSent number of bytes received with this message
 *
 * @param bytesReceived number of bytes received with this message
 */
typedef void ( * SCOREP_Substrates_MpiCollectiveEndCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    SCOREP_MpiRank                   rootRank,
    SCOREP_CollectiveType            collectiveType,
    uint64_t                         bytesSent,
    uint64_t                         bytesReceived );

/**
 * Called from MPI adapter when an MPI_Isend is completed
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * Previously there should be a SCOREP_Substrates_MpiIsendCb with the same requestId
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param requestId request ID of the MPI_Isend (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiIsendCompleteCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

/**
 * Called from MPI adapter when an MPI_Irecv is initialized
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * Previously there should be a SCOREP_Substrates_MpiIrecvCb with the same requestId
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param requestId request ID of the MPI_Irecv (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiIrecvRequestCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

/**
 * Called from MPI adapter when the status of a non-blocking communication is tested
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * Previously there should be an initialization of the non-blocking communication with the same requestId
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param requestId request ID of the non blocking communication to be tested (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiRequestTestedCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

/**
 * Called from MPI adapter when a non-blocking communication is cancelled
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * Previously there should be an initialization of the non-blocking communication with the same requestId
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param requestId request ID of the non blocking communication to be canceled (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiRequestCancelledCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

/**
 * Initialize a non blocking send via MPI
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param destinationRank rank that will receive the message
 *
 * @param communicatorHandle communicator of this location and the target rank
 *
 * @param tag MPI tag
 *
 * @param bytesSent number of sent bytes
 *
 * @param requestId request ID of the non blocking communication to be canceled (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiIsendCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   destinationRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesSent,
    SCOREP_MpiRequestId              requestId );

/**
 * Finishes an MpiIrecvRequest
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which sends an MPI message
 *
 * @param timestamp timestamp for this event
 *
 * @param sourceRank rank that sent the message
 *
 * @param communicatorHandle communicator of this location and the source rank
 *
 * @param tag MPI tag
 *
 * @param bytesReceived number of bytes received with this message
 *
 * @param requestId request ID of the non blocking communication to be canceled (see MPI standard)
 */
typedef void ( * SCOREP_Substrates_MpiIrecvCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   sourceRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesReceived,
    SCOREP_MpiRequestId              requestId );

/**
 * create a remote memory access window
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle previously defined window handle
 */
typedef void ( * SCOREP_Substrates_RmaWinCreateCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle );

/**
 * destroy a remote memory access window
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle previously defined and created window handle
 */
typedef void ( * SCOREP_Substrates_RmaWinDestroyCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle );

/**
 * begin a collective operation on an MPI remote memory access window
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 * More information is passed with the next call SCOREP_Substrates_RmaCollectiveEndCb
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 */
typedef void ( * SCOREP_Substrates_RmaCollectiveBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp );

/**
 * end a collective operation on an MPI remote memory access window
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param collectiveOp type of the collective operation
 *
 * @param syncLevel synchronization level
 *
 * @param windowHandle the previously defined and created window handle
 *
 * @param root Root process/rank if there is one
 *
 * @param bytesSent number of bytes sent
 *
 * @param bytesReceived number of bytes received
 */
typedef void ( * SCOREP_Substrates_RmaCollectiveEndCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_CollectiveType   collectiveOp,
    SCOREP_RmaSyncLevel     syncLevel,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                root,
    uint64_t                bytesSent,
    uint64_t                bytesReceived );

/**
 * An attempt to acquire a lock which turns out negative can be marked
 * with SCOREP_Substrates_RmaTryLockCb. In this case, no release record may follow.
 * With this a series of unsuccessful locking attempts can be identified.
 * If an lock attempt is successful, it is marked with
 * @a SCOREP_Substrates_RmaAcquireLockCb right away instead of a pair of
 * @a SCOREP_Substrates_RmaTryLockCb and @ SCOREP_Substrates_RmaAcquireLockCb.
 * see also the MPI specifications at https://www.mpi-forum.org/docs/
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle the previously defined and created window handle
 *
 * @param remote Rank of target in context of window
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
typedef void ( * SCOREP_Substrates_RmaTryLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                lockId,
    SCOREP_LockType         lockType );

/**
 * Marks the time that a lock is granted. This is the typical situation.
 * It has to be followed by a matching @a SCOREP_Substrates_RmaRequestLockCb record
 * later on.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
typedef void ( * SCOREP_Substrates_RmaAcquireLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                lockId,
    SCOREP_LockType         lockType );


/**
 * This record marks the time that a request for a lock is issued where
 * the RMA model ensures that the lock is granted eventually without
 * further notification. As of now this is specific for MPI. In this case,
 * the @a SCOREP_RmaAcquireLock event is not present.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
typedef void ( * SCOREP_Substrates_RmaRequestLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                lockId,
    SCOREP_LockType         lockType );


/**
 * Marks the time the lock is freed. It contains all fields that are
 * necessary to match it to either an earlier @a SCOREP_Substrates_RmaAcquireLockCb or
 * @a SCOREP_Substrates_RmaRequestLockCb event and is required to follow either of the
 * two.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 */
typedef void ( * SCOREP_Substrates_RmaReleaseLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                lockId );

/**
 * This record marks a simple pairwise synchronization.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param syncType Type of direct RMA synchronization call
 * (e.g. SCOREP_RMA_SYNC_TYPE_MEMORY, SCOREP_RMA_SYNC_TYPE_NOTIFY_IN,
 * SCOREP_RMA_SYNC_TYPE_NOTIFY_OUT).
 */
typedef void ( * SCOREP_Substrates_RmaSyncCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    SCOREP_RmaSyncType      syncType );

/**
 * This record marks the synchronization of a sub-group of the locations
 * associated with the given memory window. It needs to be recorded for
 * all participating locations.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param syncLevel Synchronization level.
 *
 * @param windowHandle Memory window.
 *
 * @param groupHandle Group of participating processes or threads.
 */
typedef void ( * SCOREP_Substrates_RmaGroupSyncCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaSyncLevel     syncLevel,
    SCOREP_RmaWindowHandle  windowHandle,
    SCOREP_GroupHandle      groupHandle );

/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param bytes Number of bytes transferred.
 *
 * @param matchingId Matching number.
 *
 * @note The matching number allows to reference the point of completion
 * of the operation. It will reappear in a completion record on the same
 * location.
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_RmaPutCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                bytes,
    uint64_t                matchingId );

typedef void ( * SCOREP_Substrates_RmaGetCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    uint64_t                bytes,
    uint64_t                matchingId );

/** @} */


/**
 * The atomic RMA operations are similar to the get and put operations.
 * As an additional field they provide the type of operation. Depending
 * on the type, data may be received, sent, or both, therefore, the
 * sizes are specified separately. Matching the local and optionally
 * remote completion works the same way as for get and put operations.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param type Type of atomic operation (see @a SCOREP_RmaAtomicType).
 *
 * @param bytesSent Number of bytes transferred to remote target.
 *
 * @param bytesReceived Number of bytes transferred from remote target.
 *
 * @param matchingId Matching number.
 *
 */

typedef void ( * SCOREP_Substrates_RmaAtomicCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint32_t                remote,
    SCOREP_RmaAtomicType    type,
    uint64_t                bytesSent,
    uint64_t                bytesReceived,
    uint64_t                matchingId );

/**
 * The SCOREP_EVENT_RMA_WAIT_CHANGE event marks a synchronization point that
 * blocks until a remote operation modifies a given memory field. This
 * event marks the beginning of the waiting period. The memory field in
 * question is part of the specified window.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 */
typedef void ( * SCOREP_Substrates_RmaWaitChangeCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle );

/**
 * The completion records mark the end of RMA operations. Local
 * completion for every RMA operation (get, put, or atomic operation)
 * always has to be marked with either @a SCOREP_Substrates_RmaOpCompleteBlockingCb or
 * @a SCOREP_Substrates_RmaOpCompleteNonBlockingCb using the same matching number as
 * the RMA operation record. An RMA operation is blocking when the
 * operation completes locally before leaving the call, for non-blocking
 * operations local completion has to be ensured by a subsequent call.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param matchingId Matching number.
 *
 * @{
 */

typedef void ( * SCOREP_Substrates_RmaOpCompleteBlockingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint64_t                matchingId );

typedef void ( * SCOREP_Substrates_RmaOpCompleteNonBlockingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint64_t                matchingId );

/** @} */


/**
 * This record indicates a test for completion. It is only useful for
 * non-blocking RMA calls where the API supports such a test. The test
 * record stands for a negative outcome, otherwise a completion record
 * is written (see @a SCOREP_Substrates_RmaOpCompleteRemoteCb).
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param matchingId Matching number.
 */
typedef void ( * SCOREP_Substrates_RmaOpTestCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint64_t                matchingId );


/**
 * An optional remote completion point can be specified with
 * SCOREP_Substrates_RmaOpCompleteRemoteCb. It is recorded on the same location as
 * the RMA operation itself. Again, multiple RMA operations may map to
 * the same SCOREP_Substrates_RmaOpCompleteRemoteCb. The target locations are not
 * explicitly specified but implicitly as all those that were referenced
 * in matching RMA operations.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param windowHandle Memory window.
 *
 * @param matchingId Matching number.
 */
typedef void ( * SCOREP_Substrates_RmaOpCompleteRemoteCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RmaWindowHandle  windowHandle,
    uint64_t                matchingId );


/**
 * Process a thread acquire/release lock event in the measurement system.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm the underlying parallelization paradigm of the lock
 *
 * @param lockId A unique ID to identify the lock, maintained by
 *               the caller.
 * @param acquisitionOrder A monotonically increasing id to determine the
 *                         order of lock acquisitions. Same for corresponding
 *                         acquire-release events.
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_ThreadAcquireLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm,
    uint32_t                lockId,
    uint32_t                acquisitionOrder );


typedef void ( * SCOREP_Substrates_ThreadReleaseLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm,
    uint32_t                lockId,
    uint32_t                acquisitionOrder );

/** @} */

/**
 * Trigger a counter, which represents more or less a metric
 * @see also SCOREP_User_TriggerMetricInt64 SCOREP_User_TriggerMetricUint64 SCOREP_User_TriggerMetricDouble
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param counterHandle previously defined counter handle
 *
 * @param value value of the counter when triggered. The datatype depends on the called function
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_TriggerCounterInt64Cb )(
    struct SCOREP_Location*  location,
    uint64_t                 timestamp,
    SCOREP_SamplingSetHandle counterHandle,
    int64_t                  value );

typedef void ( * SCOREP_Substrates_TriggerCounterUint64Cb )(
    struct SCOREP_Location*  location,
    uint64_t                 timestamp,
    SCOREP_SamplingSetHandle counterHandle,
    uint64_t                 value );

typedef void ( * SCOREP_Substrates_TriggerCounterDoubleCb )(
    struct SCOREP_Location*  location,
    uint64_t                 timestamp,
    SCOREP_SamplingSetHandle counterHandle,
    double                   value );
/** @} */


/**
 * Trigger a user defined parameter with a specific value
 * @see also SCOREP_User_ParameterInt64 SCOREP_User_ParameterUint64
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param parameterHandle previously defined parameter handle
 *
 * @param value value of the parameter when triggered. The datatype depends on the called function
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_TriggerParameterInt64Cb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParameterHandle  parameterHandle,
    int64_t                 value );

typedef void ( * SCOREP_Substrates_TriggerParameterUint64Cb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParameterHandle  parameterHandle,
    uint64_t                value );
/** @} */

/**
 * Trigger a user defined parameter with a specific value
 * @see also SCOREP_User_ParameterString
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param parameterHandle previously defined parameter handle
 *
 * @param string_handle previously defined string when the parameter is triggered.
 *
 */
typedef void ( * SCOREP_Substrates_TriggerParameterStringCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParameterHandle  parameterHandle,
    SCOREP_StringHandle     string_handle );

/**
 * called from threading instrumentation adapters before a thread team is forked, e.g., before an OpenMP parallel region
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm threading paradigm
 *
 * @param nRequestedThreads number of threads to be forked.
 *  Note that this does not necessarily represent actual threads
 *  but threads can also be reused, e..g, in OpenMP runtimes.
 *
 * @param forkSequenceCount an increasing number, unique for each process that
 *   allows to identify a parallel region
 *
 */

typedef void ( * SCOREP_Substrates_ThreadForkJoinForkCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm,
    uint32_t                nRequestedThreads,
    uint32_t                forkSequenceCount );

/**
 * called from threading instrumentation after a thread team is joined, e.g., after an OpenMP parallel region
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm threading paradigm
 *
 */
typedef void ( * SCOREP_Substrates_ThreadForkJoinJoinCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm );

/**
 * called from threading instrumentation after a thread team is created/before it is joined.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm threading paradigm
 *
 * @param threadTeam previously defined thread team
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_ThreadForkJoinTeamBeginCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam );

typedef void ( * SCOREP_Substrates_ThreadForkJoinTeamEndCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam );

/** @} */



/**
 * Process a task create event in the measurement system.
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm         One of the predefined threading models.
 *
 * @param threadTeam previously defined thread team
 *
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 *
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 *
 */
typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskCreateCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber );

/**
 * Process a task switch event
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param metricValues synchronous metrics
 *
 * @param paradigm         One of the predefined threading models.
 *
 * @param threadTeam previously defined thread team
 *
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 *
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 *
 * @param taskHandle       A handle to the resumed task.
 */

typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskSwitchCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    uint64_t*                        metricValues,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber,
    SCOREP_TaskHandle                taskHandle );

/**
 * Process a task begin/end event
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param metricValues synchronous metrics
 *
 * @param paradigm         One of the predefined threading models.
 *
 * @param regionHandle     Region handle of the task region.
 *
 * @param threadTeam previously defined thread team
 *
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 *
 * @param generationNumber The sequence number for this task. Each task created
 *                         gets a thread private generation number attached.
 *                         Combined with the @a threadId, this constitutes a
 *                         unique task ID inside the parallel region.
 *
 * @param taskHandle       A handle to the executed task
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskBeginCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_RegionHandle              regionHandle,
    uint64_t*                        metricValues,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber,
    SCOREP_TaskHandle                taskHandle );

typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskEndCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_RegionHandle              regionHandle,
    uint64_t*                        metricValues,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber,
    SCOREP_TaskHandle                taskHandle );

/** @} */

/**
 * process a thread event for a create/wait thread model instrumentation adapter e.g., pthreads
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param paradigm         One of the predefined threading models.
 *
 * @param threadTeam previously defined thread team
 *
 * @param createSequenceCount a process unique increasing number that is increased at every
 * SCOREP_Substrates_ThreadCreateWaitCreateCb and SCOREP_Substrates_ThreadForkJoinForkCb
 *
 * @{
 */

typedef void ( * SCOREP_Substrates_ThreadCreateWaitCreateCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         createSequenceCount );

typedef void ( * SCOREP_Substrates_ThreadCreateWaitWaitCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         createSequenceCount );

typedef void ( * SCOREP_Substrates_ThreadCreateWaitBeginCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         createSequenceCount );

typedef void ( * SCOREP_Substrates_ThreadCreateWaitEndCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         createSequenceCount );

/** @} */

/**
 * Event for allocating memory using malloc/calloc
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param addrAllocated allocated address, should be converted to void*
 *
 * @param bytesAllocated number of bytes allocated
 *
 * @param substrateData Internal substrates may register data for this address. They should use their substrate id to access their specific item. Substrate plugins should NOT access this variable.
 *
 * @param bytesAllocatedMetric The total size of the metric. E.g., all memory regions tracked with the memory adapters count into a specific metric
 *
 * @param bytesAllocatedProcess total number of bytes allocated in the process
 *
 */
typedef void ( * SCOREP_Substrates_TrackAllocCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    uint64_t                addrAllocated,
    size_t                  bytesAllocated,
    void*                   substrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );

/**
 * Event for allocating/freeing memory using realloc
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param oldAddr address passed to realloc
 *
 * @param oldBytesAllocated size allocated to oldAddr before calling realloc
 *
 * @param oldSubstrateData  Internal substrates get their previously registered data for oldAddr. They should use their substrate id to access their specific item. Substrate plugins should NOT access this variable.
 *
 * @param newAddr address gained from realloc
 *
 * @param newBytesAllocated size of object at newAddr after realloc
 *
 * @param newSubstrateData  Internal substrates can register data for newAddr. They should use their substrate id to access their specific item. Substrate plugins should NOT access this variable.
 *
 * @param bytesAllocatedMetric The total size of the metric. E.g., all memory regions tracked with the memory adapters count into a specific metric
 *
 * @param bytesAllocatedProcess total number of bytes allocated in the process
 *
 */
typedef void ( * SCOREP_Substrates_TrackReallocCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    uint64_t                oldAddr,
    size_t                  oldBytesAllocated,
    void*                   oldSubstrateData[],
    uint64_t                newAddr,
    size_t                  newBytesAllocated,
    void*                   newSubstrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );

/**
 * Event for freeing memory using free
 *
 * @param location location which creates this event
 *
 * @param timestamp timestamp for this event
 *
 * @param addrFreed address passed to free
 *
 * @param bytesFreed number of bytes freed
 *
 * @param substrateData Internal substrates get previously registered data for addrFreed. They should use their substrate id to access their specific item. Substrate plugins should NOT access this variable.
 *
 * @param bytesAllocatedMetric The total size of the metric. E.g., all memory regions tracked with the memory adapters count into a specific metric
 *
 * @param bytesAllocatedProcess total number of bytes allocated in the process
 *
 */
typedef void ( * SCOREP_Substrates_TrackFreeCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    uint64_t                addrFreed,
    size_t                  bytesFreed,
    void*                   substrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );


/**
 * Used to record metrics. Provided by substrates as arguments to
 * SCOREP_Metric_WriteStrictlySynchronousMetrics(),
 * SCOREP_Metric_WriteSynchronousMetrics(), and
 * SCOREP_Metric_WriteAsynchronousMetrics(). These functions are supposed
 * to be called during following events: [CallingContext]Enter,
 * [CallingContext]Exit, Sample. Will also be used for writing post mortem
 * asynchronous metrics during SCOREP_EVENT_WRITE_POST_MORTEM_METRICS.
 *
 * @param location                 A pointer to the thread location data of the thread that executed
 *                                 the metric event.
 * @param timestamp                The timestamp, when the metric event occurred.
 * @param samplingSet              The sampling set with metrics
 * @param metricValues             Array of the metric values.
 *
 */
typedef void ( * SCOREP_Substrates_WriteMetricsCb )(
    struct SCOREP_Location*  location,
    uint64_t                 timestamp,
    SCOREP_SamplingSetHandle samplingSet,
    const uint64_t*          metricValues );


/**
 * Records the creation of a new active I/O handle that can be used by subsequent I/O operation events.
 * An IoHandle is active between a pair of consecutive IoCreateHandle and IoDestroyHandle events.
 * All locations of a location group have access to an active IoHandle.
 *
 * @param location                 The location which creates this event.
 * @param timestamp                The timestamp, when the event event occurred.
 * @param handle                   A reference to the affected I/O handle which will be activated by this record.
 * @param mode                     Determines which I/O operations can be applied to this I/O handle (e.g., read-only, write-only, read-write)
 * @param creationFlags            Requested I/O handle creation flags (e.g., create, exclusive, etc.).
 * @param statusFlags              I/O handle status flags which will be associated with the handle attribute (e.g., append, create, close-on-exec, async, etc).
 */
typedef void ( * SCOREP_Substrates_IoCreateHandleCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_IoAccessMode     mode,
    SCOREP_IoCreationFlag   creationFlags,
    SCOREP_IoStatusFlag     statusFlags );

/**
 * Records the end of an active I/O handle's lifetime.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   A reference to the affected I/O handle which will be activated by this record.
 */
typedef void ( * SCOREP_Substrates_IoDestroyHandleCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle );

/**
 * Records the duplication of an already existing active I/O handle.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param oldHandle                An active I/O handle.
 * @param newHandle                A previously inactive I/O handle which will be activated by this record.
 * @param statusFlags              The status flag for the new I/O handle newHandle. No status flags will be inherited from the I/O handle oldHandle.
 */
typedef void ( * SCOREP_Substrates_IoDuplicateHandleCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   oldHandle,
    SCOREP_IoHandleHandle   newHandle,
    SCOREP_IoStatusFlag     statusFlags );

/**
 * Records a change of the position, e.g., within a file.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param offsetRequest            Requested offset.
 * @param whence                   Position inside the file from where offsetRequest should be applied (e.g., absolute from the start or end, relative to the current position).
 * @param offsetResult             Resulting offset, e.g., within the file relative to the beginning of the file.
 */
typedef void ( * SCOREP_Substrates_IoSeekCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    int64_t                 offsetRequest,
    SCOREP_IoSeekOption     whence,
    uint64_t                offsetResult );

/**
 * Records a change to the status flags associated with an active I/O handle.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param statusFlags              Set flags (e.g., close-on-exec, append, etc.).
 */
typedef void ( * SCOREP_Substrates_IoChangeStatusFlagsCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_IoStatusFlag     statusFlags );

/**
 * Records the deletion of an I/O file.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param ioParadigm               The I/O paradigm which induced the deletion.
 * @param ioFile                   File identifier.
 */
typedef void ( * SCOREP_Substrates_IoDeleteFileCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoParadigmType   ioParadigm,
    SCOREP_IoFileHandle     ioFile );

/**
 * Records the begin of a file operation (read, write, etc.).
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param mode                     Mode of an I/O handle operation (e.g., read or write).
 * @param operationFlags           Special semantic of this operation.
 * @param bytesRequest             Requested bytes to write/read.
 * @param matchingId               Identifier used to correlate associated event records of an I/O operation. This identifier is unique for the referenced I/O handle.
 */
typedef void ( * SCOREP_Substrates_IoOperationBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_IoOperationMode  mode,
    SCOREP_IoOperationFlag  operationFlags,
    uint64_t                bytesRequest,
    uint64_t                matchingId );

/**
 * Records the successful initiation of a non-blocking operation (read, write etc.) on an active I/O handle.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param matchingId               Identifier used to correlate associated event records of an I/O operation. This identifier is unique for the referenced I/O handle.
 */
typedef void ( * SCOREP_Substrates_IoOperationIssuedCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    uint64_t                matchingId );

/**
 * Records an unsuccessful test whether an I/O operation has already finished.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param matchingId               Identifier used to correlate associated event records of an I/O operation. This identifier is unique for the referenced I/O handle.
 */
typedef void ( * SCOREP_Substrates_IoOperationTestCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    uint64_t                matchingId );

/**
 * Records the end of a file operation (read, write etc.) on an active I/O handle.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param mode                     Mode of an I/O handle operation (e.g., read or write).
 * @param bytesResult              Number of actual transferred bytes.
 * @param matchingId               Identifier used to correlate associated event records of an I/O operation. This identifier is unique for the referenced I/O handle.
 */
typedef void ( * SCOREP_Substrates_IoOperationCompleteCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_IoOperationMode  mode,
    uint64_t                bytesResult,
    uint64_t                matchingId );

/**
 * Records the successful cancellation of a non-blocking operation (read, write etc.) on an active I/O handle.
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param matchingId               Identifier used to correlate associated event records of an I/O operation. This identifier is unique for the referenced I/O handle.
 */
typedef void ( * SCOREP_Substrates_IoOperationCancelledCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    uint64_t                matchingId );

/**
 * Process I/O lock events (e.g., apply or remove a POSIX advisory lock on an open file)
 *
 * @param location                 The location where this event happened.
 * @param timestamp	               The time when this event happened.
 * @param handle                   An active I/O handle.
 * @param lockType                 Type of the lock (e.g., exclusive or shared.
 *
 * @{
 */
typedef void ( * SCOREP_Substrates_IoAcquireLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_LockType         lockType );

typedef void ( * SCOREP_Substrates_IoReleaseLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_LockType         lockType );

typedef void ( * SCOREP_Substrates_IoTryLockCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_IoHandleHandle   handle,
    SCOREP_LockType         lockType );

/**
 * @}
 */

#endif /* SCOREP_SUBSTRATE_EVENTS_H */
