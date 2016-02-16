/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SUBSTRATES_DEFINITION_H
#define SCOREP_SUBSTRATES_DEFINITION_H

/**
 * @file
 *
 * This file contains datatype definitions for SCOREP substrate handling
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Macro defining substrate call construct
 */
#define SCOREP_CALL_SUBSTRATE( Event, EVENT, ARGS ) \
    do \
    { \
        SCOREP_Substrates_##Event##Cb * substrate_cb = ( SCOREP_Substrates_##Event##Cb* )scorep_substrates[ SCOREP_EVENT_##EVENT ]; \
        while ( *substrate_cb ) \
        { \
            ( *substrate_cb )ARGS; \
            ++substrate_cb; \
        } \
    } while ( 0 )

/**
 * Macro initializing substrate callback
 */
#ifdef __GNUC__
#define SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_EVENT_##EVENT ] = \
        __builtin_choose_expr( \
            __builtin_types_compatible_p( __typeof__( 0 ? ( cb ) : ( cb ) ), \
                                          SCOREP_Substrates_##Event##Cb ), \
            ( SCOREP_Substrates_Callback )( cb ), ( cb ) )
#else
#define SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Event, EVENT, cb ) \
    [ SCOREP_EVENT_##EVENT ] = ( SCOREP_Substrates_Callback )( cb )
#endif

/**
 * Generic void function pointer for substrate functions.
 */
typedef void ( * SCOREP_Substrates_Callback )( void );


/**
 * Substrates need to provide two sets of callbacks for the modes
 * SCOREP_SUBSTATES_RECORDING_ENABLED and SCOREP_SUBSTRATES_RECORDING_DISABLED.
 * This enum is used as an array index.
 */
typedef enum SCOREP_Substrates_Mode
{
    SCOREP_SUBSTRATES_RECORDING_ENABLED = 0,
    SCOREP_SUBSTRATES_RECORDING_DISABLED,

    SCOREP_SUBSTRATES_NUM_MODES
} SCOREP_Substrates_Mode;


/**
 * Substrate events. List every event that is going to be used by the
 * substrate mechanism.
 */
typedef enum SCOREP_Substrates_EventType
{
    SCOREP_EVENT_INIT_SUBSTRATE = 0,
    SCOREP_EVENT_FINALIZE_SUBSTRATE,
    SCOREP_EVENT_ENABLE_RECORDING,
    SCOREP_EVENT_DISABLE_RECORDING,
    SCOREP_EVENT_ON_LOCATION_CREATION,
    SCOREP_EVENT_ON_LOCATION_DELETION,
    SCOREP_EVENT_ON_LOCATION_ACTIVATION,
    SCOREP_EVENT_ON_LOCATION_DEACTIVATION,
    SCOREP_EVENT_PRE_UNIFY_SUBSTRATE,
    SCOREP_EVENT_WRITE_DATA,
    SCOREP_EVENT_CORE_TASK_CREATE,
    SCOREP_EVENT_CORE_TASK_COMPLETE,
    SCOREP_EVENT_INITIALIZE_MPP,
    SCOREP_EVENT_ENSURE_GLOBAL_ID,
    SCOREP_EVENT_ON_TRACING_BUFFER_FLUSH_BEGIN,
    SCOREP_EVENT_ON_TRACING_BUFFER_FLUSH_END,
    SCOREP_EVENT_ENTER_REGION,
    SCOREP_EVENT_EXIT_REGION,
    SCOREP_EVENT_SAMPLE,
    SCOREP_EVENT_CALLING_CONTEXT_ENTER,
    SCOREP_EVENT_CALLING_CONTEXT_EXIT,
    SCOREP_EVENT_ENTER_REWIND_REGION,
    SCOREP_EVENT_EXIT_REWIND_REGION,
    SCOREP_EVENT_MPI_SEND,
    SCOREP_EVENT_MPI_RECV,
    SCOREP_EVENT_MPI_COLLECTIVE_BEGIN,
    SCOREP_EVENT_MPI_COLLECTIVE_END,
    SCOREP_EVENT_MPI_ISEND_COMPLETE,
    SCOREP_EVENT_MPI_IRECV_REQUEST,
    SCOREP_EVENT_MPI_REQUEST_TESTED,
    SCOREP_EVENT_MPI_REQUEST_CANCELLED,
    SCOREP_EVENT_MPI_ISEND,
    SCOREP_EVENT_MPI_IRECV,
    SCOREP_EVENT_RMA_WIN_CREATE,
    SCOREP_EVENT_RMA_WIN_DESTROY,
    SCOREP_EVENT_RMA_COLLECTIVE_BEGIN,
    SCOREP_EVENT_RMA_COLLECTIVE_END,
    SCOREP_EVENT_RMA_TRY_LOCK,
    SCOREP_EVENT_RMA_ACQUIRE_LOCK,
    SCOREP_EVENT_RMA_REQUEST_LOCK,
    SCOREP_EVENT_RMA_RELEASE_LOCK,
    SCOREP_EVENT_RMA_SYNC,
    SCOREP_EVENT_RMA_GROUP_SYNC,
    SCOREP_EVENT_RMA_PUT,
    SCOREP_EVENT_RMA_GET,
    SCOREP_EVENT_RMA_ATOMIC,
    SCOREP_EVENT_RMA_WAIT_CHANGE,
    SCOREP_EVENT_RMA_OP_COMPLETE_BLOCKING,
    SCOREP_EVENT_RMA_OP_COMPLETE_NON_BLOCKING,
    SCOREP_EVENT_RMA_OP_TEST,
    SCOREP_EVENT_RMA_OP_COMPLETE_REMOTE,
    SCOREP_EVENT_THREAD_ACQUIRE_LOCK,
    SCOREP_EVENT_THREAD_RELEASE_LOCK,
    SCOREP_EVENT_TRIGGER_COUNTER_INT64,
    SCOREP_EVENT_TRIGGER_COUNTER_UINT64,
    SCOREP_EVENT_TRIGGER_COUNTER_DOUBLE,
    SCOREP_EVENT_TRIGGER_PARAMETER_INT64,
    SCOREP_EVENT_TRIGGER_PARAMETER_UINT64,
    SCOREP_EVENT_TRIGGER_PARAMETER_STRING,
    SCOREP_EVENT_THREAD_FORK_JOIN_FORK,
    SCOREP_EVENT_THREAD_FORK_JOIN_JOIN,
    SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_BEGIN,
    SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_END,
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_CREATE,
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_SWITCH,
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_BEGIN,
    SCOREP_EVENT_THREAD_FORK_JOIN_TASK_END,
    SCOREP_EVENT_THREAD_CREATE_WAIT_CREATE,
    SCOREP_EVENT_THREAD_CREATE_WAIT_WAIT,
    SCOREP_EVENT_THREAD_CREATE_WAIT_BEGIN,
    SCOREP_EVENT_THREAD_CREATE_WAIT_END,
    SCOREP_EVENT_ADD_ATTRIBUTE,
    SCOREP_EVENT_TRACK_ALLOC,
    SCOREP_EVENT_TRACK_REALLOC,
    SCOREP_EVENT_TRACK_FREE,
    SCOREP_EVENT_LEAKED_MEMORY,

    SCOREP_SUBSTRATES_NUM_EVENTS
} SCOREP_Substrates_EventType;

/**
 * Number of the substrates for a particular event. Currently we support at
 * most three simultaneous substrates: profiling, tracing and invalidate property.
 * See SCOREP_Substrates_DisableRecording for the example where three simultaneous
 * substrates are used.
 */
#define SCOREP_SUBSTRATES_NUM_SUBSTRATES 3

/* Global array holding the currently active substrate callbacks per event.
 * See 'enum SCOREP_Substrates_EventType'. The last element for each event is always NULL
 * and serves as the while-loop's exit condition. Consider read-only except
 * in SCOREP_Substrates_Management.c. */
extern SCOREP_Substrates_Callback scorep_substrates[ SCOREP_SUBSTRATES_NUM_EVENTS ][ SCOREP_SUBSTRATES_NUM_SUBSTRATES + 1 ];


struct SCOREP_Location;

// Per-event function pointer prototypes
typedef void ( * SCOREP_Substrates_InitSubstrateCb )(
    size_t substrateId );

typedef size_t ( * SCOREP_Substrates_FinalizeSubstrateCb )( void );

typedef void ( * SCOREP_Substrates_EnableRecordingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_DisableRecordingCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_OnLocationCreationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation );

typedef void ( * SCOREP_Substrates_OnLocationDeletionCb )(
    struct SCOREP_Location* location );

typedef void ( * SCOREP_Substrates_OnLocationActivationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation,
    uint32_t                forkSequenceCount );

typedef void ( * SCOREP_Substrates_OnLocationDeactivationCb )(
    struct SCOREP_Location* location,
    struct SCOREP_Location* parentLocation );

typedef void ( * SCOREP_Substrates_PreUnifySubstrateCb )( void );

typedef void ( * SCOREP_Substrates_WriteDataCb )( void );

typedef void ( * SCOREP_Substrates_CoreTaskCreateCb )(
    struct SCOREP_Location* locationData,
    SCOREP_TaskHandle       taskHandle );

typedef void ( * SCOREP_Substrates_CoreTaskCompleteCb )(
    struct SCOREP_Location* locationData,
    SCOREP_TaskHandle       taskHandle );

typedef void ( * SCOREP_Substrates_InitializeMppCb )( void );

typedef void ( * SCOREP_Substrates_EnsureGlobalIdCb )(
    struct SCOREP_Location* location );

typedef void ( * SCOREP_Substrates_OnTracingBufferFlushBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_OnTracingBufferFlushEndCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_EnterRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_ExitRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle,
    uint64_t*               metricValues );

typedef void ( * SCOREP_Substrates_SampleCb )(
    struct SCOREP_Location*         location,
    uint64_t                        timestamp,
    SCOREP_CallingContextHandle     callingContext,
    SCOREP_CallingContextHandle     previousCallingContext,
    uint32_t                        unwindDistance,
    SCOREP_InterruptGeneratorHandle interruptGeneratorHandle,
    uint64_t*                       metricValues );

typedef void ( * SCOREP_Substrates_CallingContextEnterCb )(
    struct SCOREP_Location*     location,
    uint64_t                    timestamp,
    SCOREP_CallingContextHandle callingContext,
    SCOREP_CallingContextHandle previousCallingContext,
    uint32_t                    unwindDistance,
    uint64_t*                   metricValues );

typedef void ( * SCOREP_Substrates_CallingContextExitCb )(
    struct SCOREP_Location*     location,
    uint64_t                    timestamp,
    SCOREP_CallingContextHandle callingContext,
    SCOREP_CallingContextHandle previousCallingContext,
    uint32_t                    unwindDistance,
    uint64_t*                   metricValues );

typedef void ( * SCOREP_Substrates_EnterRewindRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_RegionHandle     regionHandle );

typedef void ( * SCOREP_Substrates_ExitRewindRegionCb )(
    struct SCOREP_Location* location,
    uint64_t                leavetimestamp,
    SCOREP_RegionHandle     regionHandle,
    bool                    do_rewind );

typedef void ( * SCOREP_Substrates_MpiSendCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   destinationRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesSent );

typedef void ( * SCOREP_Substrates_MpiRecvCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   sourceRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesReceived );

typedef void ( * SCOREP_Substrates_MpiCollectiveBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp );

typedef void ( * SCOREP_Substrates_MpiCollectiveEndCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    SCOREP_MpiRank                   rootRank,
    SCOREP_CollectiveType            collectiveType,
    uint64_t                         bytesSent,
    uint64_t                         bytesReceived );

typedef void ( * SCOREP_Substrates_MpiIsendCompleteCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

typedef void ( * SCOREP_Substrates_MpiIrecvRequestCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

typedef void ( * SCOREP_Substrates_MpiRequestTestedCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

typedef void ( * SCOREP_Substrates_MpiRequestCancelledCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_MpiRequestId     requestId );

typedef void ( * SCOREP_Substrates_MpiIsendCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   destinationRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesSent,
    SCOREP_MpiRequestId              requestId );

typedef void ( * SCOREP_Substrates_MpiIrecvCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_MpiRank                   sourceRank,
    SCOREP_InterimCommunicatorHandle communicatorHandle,
    uint32_t                         tag,
    uint64_t                         bytesReceived,
    SCOREP_MpiRequestId              requestId );

typedef void ( * SCOREP_Substrates_RmaWinCreateCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle );

typedef void ( * SCOREP_Substrates_RmaWinDestroyCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle );

typedef void ( * SCOREP_Substrates_RmaCollectiveBeginCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp );

typedef void ( * SCOREP_Substrates_RmaCollectiveEndCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_CollectiveType         collectiveOp,
    SCOREP_RmaSyncLevel           syncLevel,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      root,
    uint64_t                      bytesSent,
    uint64_t                      bytesReceived );

typedef void ( * SCOREP_Substrates_RmaTryLockCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      lockId,
    SCOREP_LockType               lockType );

typedef void ( * SCOREP_Substrates_RmaAcquireLockCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      lockId,
    SCOREP_LockType               lockType );

typedef void ( * SCOREP_Substrates_RmaRequestLockCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      lockId,
    SCOREP_LockType               lockType );

typedef void ( * SCOREP_Substrates_RmaReleaseLockCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      lockId );

typedef void ( * SCOREP_Substrates_RmaSyncCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    SCOREP_RmaSyncType            syncType );

typedef void ( * SCOREP_Substrates_RmaGroupSyncCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_RmaSyncLevel           syncLevel,
    SCOREP_InterimRmaWindowHandle windowHandle,
    SCOREP_GroupHandle            groupHandle );

typedef void ( * SCOREP_Substrates_RmaPutCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      bytes,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaGetCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    uint64_t                      bytes,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaAtomicCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint32_t                      remote,
    SCOREP_RmaAtomicType          type,
    uint64_t                      bytesSent,
    uint64_t                      bytesReceived,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaWaitChangeCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle );

typedef void ( * SCOREP_Substrates_RmaOpCompleteBlockingCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaOpCompleteNonBlockingCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaOpTestCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint64_t                      matchingId );

typedef void ( * SCOREP_Substrates_RmaOpCompleteRemoteCb )(
    struct SCOREP_Location*       location,
    uint64_t                      timestamp,
    SCOREP_InterimRmaWindowHandle windowHandle,
    uint64_t                      matchingId );

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

typedef void ( * SCOREP_Substrates_TriggerParameterStringCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParameterHandle  parameterHandle,
    SCOREP_StringHandle     string_handle );

typedef void ( * SCOREP_Substrates_ThreadForkJoinForkCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm,
    uint32_t                nRequestedThreads,
    uint32_t                forkSequenceCount );

typedef void ( * SCOREP_Substrates_ThreadForkJoinJoinCb )(
    struct SCOREP_Location* location,
    uint64_t                timestamp,
    SCOREP_ParadigmType     paradigm );

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

typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskCreateCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber );

typedef void ( * SCOREP_Substrates_ThreadForkJoinTaskSwitchCb )(
    struct SCOREP_Location*          location,
    uint64_t                         timestamp,
    uint64_t*                        metricValues,
    SCOREP_ParadigmType              paradigm,
    SCOREP_InterimCommunicatorHandle threadTeam,
    uint32_t                         threadId,
    uint32_t                         generationNumber,
    SCOREP_TaskHandle                taskHandle );

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

typedef void ( * SCOREP_Substrates_AddAttributeCb )(
    struct SCOREP_Location* location,
    SCOREP_AttributeHandle  attributeHandle,
    void*                   value );

typedef void ( * SCOREP_Substrates_TrackAllocCb )(
    struct SCOREP_Location* location,
    uint64_t                addrAllocated,
    size_t                  bytesAllocated,
    void*                   substrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );

typedef void ( * SCOREP_Substrates_TrackReallocCb )(
    struct SCOREP_Location* location,
    uint64_t                oldAddr,
    size_t                  oldBytesAllocated,
    void*                   oldSubstrateData[],
    uint64_t                newAddr,
    size_t                  newBytesAllocated,
    void*                   newSubstrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );

typedef void ( * SCOREP_Substrates_TrackFreeCb )(
    struct SCOREP_Location* location,
    uint64_t                addrFreed,
    size_t                  bytesFreed,
    void*                   substrateData[],
    size_t                  bytesAllocatedMetric,
    size_t                  bytesAllocatedProcess );

typedef void ( * SCOREP_Substrates_LeakedMemoryCb )(
    uint64_t addrLeaked,
    size_t   bytesLeaked,
    void*    substrateData[] );

#endif /* SCOREP_SUBSTRATES_DEFINITION_H */
