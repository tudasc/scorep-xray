/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_EVENTS_H
#define SCOREP_EVENTS_H


/**
 * @file
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */


#include "SCOREP_Types.h"
#include "SCOREP_Location.h"
#include "SCOREP_DefinitionHandles.h"

#include <stdbool.h>

/**
 * @defgroup SCOREP_Events SCOREP Events
 *
 * - After defining regions or other entities the adapter may call event
     functions. Most prominent are the region enter (SCOREP_EnterRegion()) and
     exit (SCOREP_ExitRegion()) functions which trigger the callpath handling,
     the metrics calculation and the trace writing. The user is responsible for
     proper nesting, i.e. that a higher level region can't be exited before
     all child regions are exited.

 * - The main difference between VampirTrace and Scalasca regarding the event
     interface is the timestamping. Where Scalasca takes the timestamp in the
     measurement system, VampirTrace requires every adapter to do this.

 * - The Scalasca way currently introduces some overhead if additional timings
     are done on events that are logically at once. Prominent examples are
     MPI_Send/Recv where the MPI adapter obtain a timestamp on esd_enter and
     on esd_mpi_send although the are logically at once. It turns out that
     this extra timestamp isn't really necessary for Scalasca's analysis
     (maybe it was at some time). So we will improve the implementation and do
     the timestamping inside the measurement system.

 * @todo MPI non-blocking, Milestone 2
 * @todo MPI RMA, Milestone 2
 * @todo MPI I/O, Milestone 2
 * @todo Interface for additional attributes, Milestone 3
 *
 */
/*@{*/



/**
 * Process a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle );





/**
 * Process a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the exit event.
 */
void
SCOREP_ExitRegion( SCOREP_RegionHandle regionHandle );




/**
 * Generate a rewind region enter event in the measurement system.
 *
 * @param regionHandle The corresponding rewind region for the enter event.
 */
void
SCOREP_EnterRewindRegion( SCOREP_RegionHandle regionHandle );

/**
 * Generate a rewind region exit event in the measurement system.
 *
 * @param regionHandle The corresponding rewind region for the exit event.
 * @param value A boolean parameter, whether the trace buffer should be
 *              rewinded (value = true) or not (value = false).
 */
void
SCOREP_ExitRewindRegion( SCOREP_RegionHandle regionHandle,
                         bool                value );


/**
 * Process a region exit event in the measurement system.
 * Special version that allows to supply a specific context instead
 * of using the current CPU thread/time/metrics.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param regionHandle The corresponding region for the exit event.
 */
void
SCOREP_Location_ExitRegion( SCOREP_Location*    location,
                            uint64_t            timestamp,
                            SCOREP_RegionHandle regionHandle );




/**
 * Process a region enter event in the measurement system.
 * Special version that allows to supply a specific context instead
 * of using the current CPU thread/time/metrics
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param regionHandle The corresponding region for the enter event
 */
void
SCOREP_Location_EnterRegion( SCOREP_Location*    location,
                             uint64_t            timestamp,
                             SCOREP_RegionHandle regionHandle );




/**
 * Process an mpi send event in the measurement system.
 *
 * @param destinationRank The MPI destination rank in the communicator
 * specified with @a communicatorHandle. Because of the scalable
 * communicator handling, it is not possible to covert ranks anymore.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @note the former globalDestinationRank has changed to a local
 * rank specification. Querying the global rank is quite expensive
 * if you are not in MPI_COMM_WORLD.
 *
 * @see SCOREP_Definitions_NewInterimCommunicator()
 */
void
SCOREP_MpiSend( SCOREP_MpiRank                   destinationRank,
                SCOREP_InterimCommunicatorHandle communicatorHandle,
                uint32_t                         tag,
                uint64_t                         bytesSent );


/**
 * Process an mpi recv event in the measurement system.
 *
 * @param sourceRank The MPI source rank in the communicator
 * specified with @a communicatorHandle. Because of the scalable
 * communicator handling, it is not possible to covert ranks anymore.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 *
 * @note the former globalSourceRank has changed to a local
 * rank specification. Querying the global rank is quite expensive
 * if you are not in MPI_COMM_WORLD.
 *
 * @todo Do measurements to compare the global/local rank performance at large
 * scale.
 */
void
SCOREP_MpiRecv( SCOREP_MpiRank                   sourceRank,
                SCOREP_InterimCommunicatorHandle communicatorHandle,
                uint32_t                         tag,
                uint64_t                         bytesReceived );


/**
 * Process an mpi collective begin event in the measurement system.
 *
 * Records also an enter event into the region @a regionHandle.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @return The used timestamp for this event.
 */
uint64_t
SCOREP_MpiCollectiveBegin( SCOREP_RegionHandle regionHandle );

/**
 * Process an mpi collective event in the measurement system.
 *
 * Records also an leave event out of the region @a regionHandle.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param rootRank Root rank of the collective operation in
 * the communicator specified with @a communicatorHandle,
 * or SCOREP_INVALID_ROOT_RANK.
 *
 * @param collectiveType The type ff the collective.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 *
 */
void
SCOREP_MpiCollectiveEnd( SCOREP_RegionHandle              regionHandle,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         SCOREP_MpiRank                   rootRank,
                         SCOREP_MpiCollectiveType         collectiveType,
                         uint64_t                         bytesSent,
                         uint64_t                         bytesReceived );

/**
 * Processs a MPI send complete event in the measurement system.
 * @param requestId The request identifier, which specifies the request that was
 *                  completed.
 */
void
SCOREP_MpiIsendComplete( SCOREP_MpiRequestId requestId );

/**
 * Processs a MPI receive request event in the measurement system.
 * @param requestId The request identifier, which specifies the new receive request.
 */
void
SCOREP_MpiIrecvRequest( SCOREP_MpiRequestId requestId );

/**
 * Process a MPI request test event in the measurement system.
 * @param requestId The request identifier, which specifies the request that was
 *                  tested.
 */
void
SCOREP_MpiRequestTested( SCOREP_MpiRequestId requestId );

/**
 * Process a MPI request cancel event in the measurement system.
 * @param requestId The request identifier, which specifies the request that was
 *                  cancelled.
 */
void
SCOREP_MpiRequestCancelled( SCOREP_MpiRequestId requestId );


/**
 * Process an mpi non-blocking send event in the measurement system.
 *
 * @param destinationRank The MPI destination rank in the communicator
 * specified with @a communicatorHandle. Because of the scalable
 * communicator handling, it is not possible to covert ranks anymore.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @param requestId The request identifier that is associated with the newly created
 * send request.
 *
 * @note the former globalDestinationRank has changed to a local
 * rank specification. Querying the global rank is quite expensive
 * if you are not in MPI_COMM_WORLD.
 *
 * @see SCOREP_Definitions_NewInterimCommunicator()
 */
void
SCOREP_MpiIsend( SCOREP_MpiRank                   destinationRank,
                 SCOREP_InterimCommunicatorHandle communicatorHandle,
                 uint32_t                         tag,
                 uint64_t                         bytesSent,
                 SCOREP_MpiRequestId              requestId );


/**
 * Process an mpi recv event in the measurement system.
 *
 * @param sourceRank The MPI source rank in the communicator
 * specified with @a communicatorHandle. Because of the scalable
 * communicator handling, it is not possible to convert ranks anymore.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 *
 * @param requestId The request identifier that is associated with the now completed
 * request.
 *
 * @note the former globalSourceRank has changed to a local
 * rank specification. Querying the global rank is quite expensive
 * if you are not in MPI_COMM_WORLD.
 */
void
SCOREP_MpiIrecv( SCOREP_MpiRank                   sourceRank,
                 SCOREP_InterimCommunicatorHandle communicatorHandle,
                 uint32_t                         tag,
                 uint64_t                         bytesReceived,
                 SCOREP_MpiRequestId              requestId );


/**
 * Mark the creation of the window on all participating processes/threads
 * and thus enclose all operations related to this window. See also
 * @a SCOREP_RmaWinDestroy.
 *
 * @param window Memory window.
 */
void
SCOREP_RmaWinCreate( SCOREP_InterimRmaWindowHandle windowHandle );


/**
 * Mark the destruction of the window on all participating processes/threads
 * and thus enclose all operations related to this window. See also
 * @a SCOREP_RmaWinCreate.
 *
 * @param window Memory window.
 */
void
SCOREP_RmaWinDestroy( SCOREP_InterimRmaWindowHandle windowHandle );


/**
 * The following event records for collective RMA operations must be
 * generated on all participating members of the communicator that is
 * referenced from the memory window. On all locations, a
 * @a SCOREP_RmaCollectiveBegin event record must be followed by a
 * @a SCOREP_RmaCollectiveEnd event record with all details. It is
 * invalid to intermix or nest begin and end records of different
 * collective operations, but local or remote completion records may be
 * placed in between.
 *
 * @{
 */
void
SCOREP_RmaCollectiveBegin();


/**
 * @param syncLevel Synchronization level.
 *
 * @param win Memory window.
 *
 * @param collectiveOp Determines type of collective operation.
 *
 * @param root Root process/rank if there is one.
 *
 * @param bytesSent Number of bytes sent.
 *
 * @param bytesReceived Number of bytes received.
 */
void
SCOREP_RmaCollectiveEnd( SCOREP_MpiCollectiveType      collectiveOp,
                         SCOREP_RmaSyncLevel           syncLevel,
                         SCOREP_InterimRmaWindowHandle windowHandle,
                         uint32_t                      root,
                         uint64_t                      bytesSent,
                         uint64_t                      bytesReceived );

/** @} */


/**
 * An attempt to acquire a lock which turns out negative can be marked
 * with SCOREP_RmaTryLock. In this case, no release record may follow.
 * With this a series of unsuccessful locking attempts can be identified.
 * If an lock attempt is successful, it is marked with
 * @a SCOREP_RmaAquireLock right away instead of a pair of
 * @a SCOREP_RmaTryLock and @ SCOREP_RmaAquireLock.
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaTryLock( SCOREP_InterimRmaWindowHandle windowHandle,
                   uint32_t                      remote,
                   uint64_t                      lockId,
                   SCOREP_LockType               lockType );


/**
 * Marks the time that a lock is granted. This is the typical situation.
 * It has to be followed by a matching @a SCOREP_RmaReleaseLock record
 * later on.
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaAcquireLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId,
                       SCOREP_LockType               lockType );


/**
 * This record marks the time that a request for a lock is issued where
 * the RMA model ensures that the lock is granted eventually without
 * further notification. As of now this is specific for MPI. In this case,
 * the @a SCOREP_RmaAquireLock event is not present.
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaRequestLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId,
                       SCOREP_LockType               lockType );


/**
 * Marks the time the lock is freed. It contains all fields that are
 * necessary to match it to either an earlier @a SCOREP_AquireLock or
 * @a SCOREP_RequestLock event and is required to follow either of the
 * two.
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaReleaseLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId );


/**
 * This record marks a simple pairwise synchronization.
 *
 * @param win Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param syncType Synchronization level (e.g. SCOREP_RMA_SYNC_LEVEL_NONE,
 * SCOREP_RMA_SYNC_LEVEL_PROCESS, SCOREP_RMA_SYNC_LEVEL_MEMORY).
 */
void
SCOREP_RmaSync( SCOREP_InterimRmaWindowHandle windowHandle,
                uint32_t                      remote,
                SCOREP_RmaSyncType            syncType );


/**
 * This record marks the synchronization of a sub-group of the locations
 * associated with the given memory window. It needs to be recorded for
 * all participants.
 *
 * @param syncLevel Synchronization level.
 *
 * @param win Memory window.
 *
 * @param group Group of participating processes or threads.
 */
void
SCOREP_RmaGroupSync( SCOREP_RmaSyncLevel           syncLevel,
                     SCOREP_InterimRmaWindowHandle windowHandle,
                     SCOREP_GroupHandle            groupHandle );


/**
 * The SCOREP_RmaWaitChange event marks a synchronization point that
 * blocks until a remote operation modifies a given memory field. This
 * event marks the beginning of the waiting period. The memory field in
 * question is part of the specified window.
 *
 * @param win Memory window.
 */
void
SCOREP_RmaWaitChange( SCOREP_InterimRmaWindowHandle windowHandle );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param win Memory window.
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
 */
void
SCOREP_RmaPut( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param win Memory window.
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
 */
void
SCOREP_RmaGet( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId );


/**
 * The atomic RMA operations are similar to the get and put operations.
 * As an additional field they provide the type of operation. Depending
 * on the type, data may be received, sent, or both, therefore, the
 * sizes are specified separately. Matching the local and optionally
 * remote completion works the same way as for get and put operations.
 *
 * @param win Window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param type Type of atomic operation (see @a SCOREP_RmaAtomicType).
 *
 * @param bytesSent Number of bytes transferred to rmeote target.
 *
 * @param bytesReceived Number of bytes transferred from remote target.
 *
 * @param matchingId Matching number.
 *
 */
void
SCOREP_RmaAtomic( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint32_t                      remote,
                  SCOREP_RmaAtomicType          type,
                  uint64_t                      bytesSent,
                  uint64_t                      bytesReceived,
                  uint64_t                      matchingId );


/**
 * The completion records mark the end of RMA operations. Local
 * completion for every RMA operation (get, put, or atomic operation)
 * always has to be marked with either @a SCOREP_RmaOpCompleteBlocking or
 * @a SCOREP_RmaOpNonCompleteBlocking using the same matching number as
 * the RMA operation record. An RMA operation is blocking when the
 * operation completes locally before leaving the call, for non-blocking
 * operations local completion has to be ensured by a subsequent call.
 *
 * @param win Memory window.
 *
 * @param matchingId Matching number.
 *
 * @{
 */
void
SCOREP_RmaOpCompleteBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                              uint64_t                      matchingId );


void
SCOREP_RmaOpCompleteNonBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint64_t                      matchingId );

/** @} */


/**
 * This record indicates a test for completion. It is only useful for
 * non-blocking RMA calls where the API supports such a test. The test
 * record stands for a negative outcome, otherwise a completion record
 * is written (see @a SCOREP_RmaOpCompleteRemote).
 *
 * @param win Memory window.
 *
 * @param matchingId Matching number.
 */
void
SCOREP_RmaOpTest( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint64_t                      matchingId );


/**
 * An optional remote completion point can be specified with
 * SCOREP_RmaOpCompleteRemote. It is recorded on the same location as
 * the RMA operation itself. Again, multiple RMA operations may map to
 * the same SCOREP_RmaOpCompleteRemote. The target locations are not
 * explicitly specified but implicitly as all those that were referenced
 * in matching RMA operations.
 *
 * @param win Memory window.
 *
 * @param matchingId Matching number.
 */
void
SCOREP_RmaOpCompleteRemote( SCOREP_InterimRmaWindowHandle windowHandle,
                            uint64_t                      matchingId );



/**
 * Mark the creation of the window on all participating processes/threads
 * and thus enclose all operations related to this window. See also
 * @ SCOREP_RmaWinDestroy.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param window Memory window.
 */
void
SCOREP_Location_RmaWinCreate( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle );


/**
 * Mark the destruction of the window on all participating processes/threads
 * and thus enclose all operations related to this window. See also
 * @ SCOREP_RmaWinCreate.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param window Memory window.
 */
void
SCOREP_Location_RmaWinDestroy( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param win Memory window.
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
 */
void
SCOREP_Location_RmaPut( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param win Memory window.
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
 */
void
SCOREP_Location_RmaGet( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId );


/**
 * The completion records mark the end of RMA operations. Local
 * completion for every RMA operation (get, put, or atomic operation)
 * always has to be marked with either @ SCOREP_RmaOpCompleteBlocking or
 * @ SCOREP_RmaOpNonCompleteBlocking using the same matching number as
 * the RMA operation record. An RMA operation is blocking when the
 * operation completes locally before leaving the call, for non-blocking
 * operations local completion has to be ensured by a subsequent call.
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 *                     Needs to be monotonic increasing for each location.
 * @param win Memory window.
 *
 * @param matchingId Matching number.
 *
 * @{
 */
void
SCOREP_Location_RmaOpCompleteBlocking( SCOREP_Location*              location,
                                       uint64_t                      timestamp,
                                       SCOREP_InterimRmaWindowHandle windowHandle,
                                       uint64_t                      matchingId );



/**
 * Notify the measurement system about the creation of a fork-join
 * parallel execution with at max @a nRequestedThreads new
 * threads. This function needs to be triggered for every thread
 * creation in a fork-join model, e.g., #pragma omp parallel in OpenMP
 * (for create-wait models see SCOREP_ThreadCreate()).
 * SCOREP_ThreadFork() needs to be called outside the parallel
 * execution from the thread creating the parallel region.
 *
 * @param model One of the predefined threading models.
 *
 * @param nRequestedThreads Upper bound of threads that comprise the
 * parallel region to be created.
 *
 * @return The process-global forkSequenceCount (starting at 0) that
 * needs to be provided in the corresponding SCOREP_ThreadTeamBegin(),
 * SCOREP_ThreadEnd() and SCOREP_ThreadJoin(), if providing by the
 * adapter is possible. You can ignore the return value and pass
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT to the mentioned
 * functions if the model implementation takes care of maintaining the
 * sequence count.
 *
 * @note All threads in the following parallel region including the
 * master/creator need to call SCOREP_ThreadTeamBegin() and
 * SCOREP_ThreadEnd().
 *
 * @note After execution of the parallel region the master/creator
 * needs to call SCOREP_ThreadJoin().
 *
 * @see SCOREP_ThreadCreate()
 */
uint32_t
SCOREP_ThreadFork( SCOREP_ThreadModel model,
                   uint32_t           nRequestedThreads );


/**
 * Notify the measurement system about the completion of a fork-join
 * parallel execution. The parallel execution was started by a call to
 * SCOREP_ThreadFork() that returned the @a forkSequenceCount that
 * needs to be provided to this function.
 *
 * @param model One of the predefined threading models.
 *
 * @param forkSequenceCount The fork sequence count returned by the
 * corresponding SCOREP_ThreadFork() or
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the latter,
 * the forkSequenceCount should be maintained in the model-specific
 * implementation, see e.g., the OpenMP implementation of
 * SCOREP_Thread_OnJoin().
 *
 * @note See the notes to SCOREP_ThreadFork().
 */
void
SCOREP_ThreadJoin( SCOREP_ThreadModel model,
                   uint32_t           forkSequenceCount );


/**
 * Notify the measurement system about the creation of a create-wait
 * parallel execution with one new thread. This function needs to be
 * triggered for every thread creation in a create-wait model, e.g.,
 * pthread_create() in Pthreads (for fork-join models see
 * SCOREP_ThreadFork()).
 *
 * @param model One of the predefined threading models.
 *
 * @return The process-global forkSequenceCount (starting at 0) that
 * needs to be provided in the corresponding SCOREP_ThreadTeamBegin(),
 * SCOREP_ThreadEnd() and SCOREP_ThreadWait(), if providing by the
 * adapter is possible. You can ignore the return value and pass
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT to the mentioned
 * functions if the model implementation takes care of maintaining the
 * sequence count.
 *
 * @note Only the created thread must call SCOREP_ThreadTeamBegin() and
 * SCOREP_ThreadEnd().
 *
 * @note Usually some other thread waits for the completion of the
 * created one (e.g., pthread_join()). This thread needs to call
 * SCOREP_ThreadWait() providing the unique fork_sequence_count
 * returned from this function. For Pthreads this is always possible
 * even if the thread is terminated using pthread_exit() or
 * pthread_cancel().
 *
 * @see SCOREP_ThreadFork()
 */
uint32_t
SCOREP_ThreadCreate( SCOREP_ThreadModel model );


/**
 * Notify the measurement system about the completion of a create-wait
 * parallel execution. The parallel execution was started by a call to
 * SCOREP_ThreadCreate() that returned the @a forkSequenceCount that
 * needs to be provided to this function.
 *
 * @param model One of the predefined threading models.
 *
 * @param forkSequenceCount The fork sequence count returned by the
 * corresponding SCOREP_ThreadCreate() or
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the latter,
 * the forkSequenceCount should be maintained in the model-specific
 * implementation.
 */
void
SCOREP_ThreadWait( SCOREP_ThreadModel model,
                   uint32_t           forkSequenceCount );


/**
 * Notify the measurement system about the begin of a parallel
 * execution on a thread created by either SCOREP_ThreadFork() or
 * SCOREP_ThreadCreate(). In case of SCOREP_ThreadFork() all created
 * threads including the master must call SCOREP_ThreadTeamBegin().
 *
 * @param model One of the predefined threading models.
 *
 * @param forkSequenceCount The forkSequenceCount returned by the
 * corresponding SCOREP_ThreadFork() or SCOREP_ThreadCreate() call
 * or SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the
 * latter, the forkSequenceCount should be maintained in the
 * model-specific implementation.
 *
 * @param threadId Id within the team of threads that constitute the
 * parallel region.
 *
 * @note The end of the parallel execution will be signalled by a call
 * to SCOREP_ThreadEnd().
 *
 * @note Per convention and as there is no parallelism for the initial
 * thread we don't call SCOREP_ThreadTeamBegin() and SCOREP_ThreadEnd()
 * for the initial thread.
 */
void
SCOREP_ThreadTeamBegin( SCOREP_ThreadModel model,
                        uint32_t           forkSequenceCount,
                        uint32_t           threadId );


/**
 * Notify the measurement system about the end of a parallel execution
 * on a thread created by either SCOREP_ThreadFork() or
 * SCOREP_ThreadCreate(). Every thread that started a parallel
 * execution via SCOREP_ThreadTeamBegin() needs to end via
 * SCOREP_ThreadEnd().
 *
 * @param forkSequenceCount The forkSequenceCount returned by the
 * corresponding SCOREP_ThreadFork() or SCOREP_ThreadCreate() call
 * or SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the
 * latter, the forkSequenceCount should be maintained in the
 * model-specific implementation.
 *
 * @param model One of the predefined threading models.
 *
 * @note The begin of the parallel execution was signalled by a call
 * to SCOREP_ThreadTeamBegin().
 *
 * @note Per convention and as there is no parallelism for the initial
 * thread we don't call SCOREP_ThreadTeamBegin() and SCOREP_ThreadEnd()
 * for the initial thread.
 */
void
SCOREP_ThreadEnd( SCOREP_ThreadModel model,
                  uint32_t           forkSequenceCount );


/**
 * Process a thread acquire lock event in the measurement system.
 *
 * @param lockId A unique ID to identify the lock. Needs to be maintained by
 *               the caller.
 * @param acquisitionOrder A monotonically increasing id to determine the
 *                         order of lock acquisitions. Same for corresponding
 *                         acquire-release events.
 */
void
SCOREP_ThreadAcquireLock( SCOREP_ThreadModel model,
                          uint32_t           lockId,
                          uint32_t           acquisitionOrder );


/**
 * Process a thread release lock event in the measurement system.
 *
 * @param lockId A unique ID to identify the lock. Needs to be maintained by
 *               the caller.
 * @param acquisitionOrder A monotonically increasing id to determine the
 *                         order of lock acquisitions. Same for corresponding
 *                         acquire-release events.
 */
void
SCOREP_ThreadReleaseLock( SCOREP_ThreadModel model,
                          uint32_t           lockId,
                          uint32_t           acquisitionOrder );


/**
 * Process a task create event in the measurement system.
 *
 * @param model            One of the predefined threading models.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 */
void
SCOREP_ThreadTaskCreate( SCOREP_ThreadModel model,
                         uint32_t           threadId,
                         uint32_t           generationNumber );


/**
 * Process a task switch event in the measurement system.
 *
 * @param model            One of the predefined threading models.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 */
void
SCOREP_ThreadTaskSwitch( SCOREP_ThreadModel model,
                         uint32_t           threadId,
                         uint32_t           generationNumber );


/**
 * Process a task begin event in the measurement system.
 *
 * @param model            One of the predefined threading models.
 * @param regionHandle     Region handle of the task region.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task created
 *                         gets a thread private generation number attached.
 *                         Combined with the @a threadId, this constitutes a
 *                         unique task ID inside the parallel region.
 */
void
SCOREP_ThreadTaskBegin( SCOREP_ThreadModel  model,
                        SCOREP_RegionHandle regionHandle,
                        uint32_t            threadId,
                        uint32_t            generationNumber );

/**
 * Process a task end event in the measurement system.
 *
 * @param model            One of the predefined threading models.
 * @param regionHandle     Region handle of the task region.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 */
void
SCOREP_ThreadTaskEnd( SCOREP_ThreadModel  model,
                      SCOREP_RegionHandle regionHandle,
                      uint32_t            threadId,
                      uint32_t            generationNumber );


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterInt64( SCOREP_SamplingSetHandle counterHandle,
                            int64_t                  value );


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterUint64( SCOREP_SamplingSetHandle counterHandle,
                             uint64_t                 value );


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterDouble( SCOREP_SamplingSetHandle counterHandle,
                             double                   value );


/**
 *
 * Special version that allows to supply a specific context instead
 * of using the current CPU thread/time/metrics
 *
 * @param location     A LocationData handle that specifies where this event is recorded.
 *                     May be NULL to record event for current location.
 * @param timestamp    Time that this event happened at.
 * @param counterHandle
 * @param value
 */
void
SCOREP_Location_TriggerCounterUint64( SCOREP_Location*         location,
                                      uint64_t                 timestamp,
                                      SCOREP_SamplingSetHandle counterHandle,
                                      uint64_t                 value );


/**
 *
 *
 * @param markerHandle
 *
 * @planned To be implemented in milestone 2
 */
void
SCOREP_TriggerMarker( SCOREP_MarkerHandle markerHandle );


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterInt64( SCOREP_ParameterHandle parameterHandle,
                              int64_t                value );


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterUint64( SCOREP_ParameterHandle parameterHandle,
                               uint64_t               value );


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterString( SCOREP_ParameterHandle parameterHandle,
                               const char*            value );


/**
 * Returns the time stamp of the last triggered event on the current location.
 *
 */
uint64_t
SCOREP_GetLastTimeStamp( void );

/*@}*/


#endif /* SCOREP_EVENTS_H */
