/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2016,
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
#include <SCOREP_DefinitionHandles.h>

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
 * Notify the measurement that a wrappers was entered.
 *
 * Needs always be called for wrapped functions. Regardless, whether the
 * function will be filterred or not. Consider using the
 * @a SCOREP_EnterWrappedRegion function, if the function is not filtered.
 * This function must directly be called by the wrapper function.
 *
 * This function should only be called if unwinding is enabled.
 *
 * @param regionHandle The region handle of the wrappee.
 */
void
SCOREP_EnterWrapper( SCOREP_RegionHandle regionHandle );


/**
 * Notify the measurement that a wrappers will be left in case the region as filterred.
 *
 * Needs only be called for wrapped functions where the enter event was filtered.
 *
 * This function should only be called if unwinding is enabled.
 *
 * @param regionHandle The region handle of the wrappee.
 */
void
SCOREP_ExitWrapper( SCOREP_RegionHandle regionHandle );

/**
 * Process a sample event in the measurement system.
 *
 * @param interruptGeneratorHandle Source generating the interrupt of this sample
 */
void
SCOREP_Sample( SCOREP_InterruptGeneratorHandle interruptGeneratorHandle,
               void*                           contextPtr );

/**
 * Trigger a sample with an invalid current calling context,
 *
 * @param location               Location for the last sample
 * @param previousCallingContext The previous calling context
 */
void
SCOREP_Location_DeactivateCpuSample( SCOREP_Location*            location,
                                     SCOREP_CallingContextHandle previousCallingContext );

/**
 * Process a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle );

/**
 * A convenience call for a combined SCOREP_EnterWrapper/SCOREP_EnterRegion
 * I.e., the region is not filtered
 * This function must directly be called by the wrapper function.
 */
void
SCOREP_EnterWrappedRegion( SCOREP_RegionHandle regionHandle );

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
 * Add an attribute to the attribute list of the current CPU location.
 *
 * @param attributeHandle Attribute reference to add.
 *
 * @param value Pointer to the value to add. The pointer type is determined using \p attributeHandle.
 */
void
SCOREP_AddAttribute( SCOREP_AttributeHandle attributeHandle,
                     void*                  value );


/**
 * Add an attribute to a location's attribute list.
 *
 * @param location Location for attribute.
 *
 * @param attributeHandle Attribute reference to add.
 *
 * @param value Pointer to the value to add. The pointer type is determined using \p attributeHandle.
 */
void
SCOREP_Location_AddAttribute( SCOREP_Location*       location,
                              SCOREP_AttributeHandle attributeHandle,
                              void*                  value );


/**
 * Add a source code location attribute to the current attribute list.
 *
 * @param file The file of the source code location.
 *
 * @param lineNumber the line number of the source code location.
 */
void
SCOREP_AddSourceCodeLocation( const char*   file,
                              SCOREP_LineNo lineNumber );


/**
 * Add a source code location attribute to the attribute list of \p location.
 *
 * @param location Location for attribute.
 *
 * @param file The file of the source code location.
 *
 * @param lineNumber the line number of the source code location.
 */
void
SCOREP_Location_AddSourceCodeLocation( SCOREP_Location* location,
                                       const char*      file,
                                       SCOREP_LineNo    lineNumber );


/**
 * Adds a location property to the current location. A property consists of a
 * key/value pair.
 * @param name   the key of the property.
 * @param value  the property value.
 */
void
SCOREP_AddLocationProperty( const char* name,
                            const char* value );


/**
 * Add a location property to a location.
 *
 * @param location Location.
 *
 * @param name Property name.
 *
 * @param value Property value.
 */
void
SCOREP_Location_AddLocationProperty( SCOREP_Location* location,
                                     const char*      name,
                                     const char*      value );


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
 */
void
SCOREP_MpiCollectiveBegin( void );

/**
 * Process an mpi collective event in the measurement system.
 *
 * Records also an leave event out of the region @a regionHandle.
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
SCOREP_MpiCollectiveEnd( SCOREP_InterimCommunicatorHandle communicatorHandle,
                         SCOREP_MpiRank                   rootRank,
                         SCOREP_CollectiveType            collectiveType,
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
SCOREP_RmaWinCreate( SCOREP_RmaWindowHandle windowHandle );


/**
 * Mark the destruction of the window on all participating processes/threads
 * and thus enclose all operations related to this window. See also
 * @a SCOREP_RmaWinCreate.
 *
 * @param window Memory window.
 */
void
SCOREP_RmaWinDestroy( SCOREP_RmaWindowHandle windowHandle );


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
SCOREP_RmaCollectiveBegin( void );


/**
 * @param syncLevel Synchronization level.
 *
 * @param windowHandle Memory window.
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
SCOREP_RmaCollectiveEnd( SCOREP_CollectiveType  collectiveOp,
                         SCOREP_RmaSyncLevel    syncLevel,
                         SCOREP_RmaWindowHandle windowHandle,
                         uint32_t               root,
                         uint64_t               bytesSent,
                         uint64_t               bytesReceived );

/** @} */


/**
 * An attempt to acquire a lock which turns out negative can be marked
 * with SCOREP_RmaTryLock. In this case, no release record may follow.
 * With this a series of unsuccessful locking attempts can be identified.
 * If an lock attempt is successful, it is marked with
 * @a SCOREP_RmaAcquireLock right away instead of a pair of
 * @a SCOREP_RmaTryLock and @ SCOREP_RmaAcquireLock.
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
SCOREP_RmaTryLock( SCOREP_RmaWindowHandle windowHandle,
                   uint32_t               remote,
                   uint64_t               lockId,
                   SCOREP_LockType        lockType );


/**
 * Marks the time that a lock is granted. This is the typical situation.
 * It has to be followed by a matching @a SCOREP_RmaReleaseLock record
 * later on.
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaAcquireLock( SCOREP_RmaWindowHandle windowHandle,
                       uint32_t               remote,
                       uint64_t               lockId,
                       SCOREP_LockType        lockType );


/**
 * This record marks the time that a request for a lock is issued where
 * the RMA model ensures that the lock is granted eventually without
 * further notification. As of now this is specific for MPI. In this case,
 * the @a SCOREP_RmaAcquireLock event is not present.
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
SCOREP_RmaRequestLock( SCOREP_RmaWindowHandle windowHandle,
                       uint32_t               remote,
                       uint64_t               lockId,
                       SCOREP_LockType        lockType );


/**
 * Marks the time the lock is freed. It contains all fields that are
 * necessary to match it to either an earlier @a SCOREP_AcquireLock or
 * @a SCOREP_RequestLock event and is required to follow either of the
 * two.
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param lockId Lock id in context of window.
 *
 * @param lockType Type of lock (shared vs. exclusive).
 */
void
SCOREP_RmaReleaseLock( SCOREP_RmaWindowHandle windowHandle,
                       uint32_t               remote,
                       uint64_t               lockId );


/**
 * This record marks a simple pairwise synchronization.
 *
 * @param windowHandle Memory window.
 *
 * @param remote Rank of target in context of window.
 *
 * @param syncType Type of direct RMA synchronization call
 * (e.g. SCOREP_RMA_SYNC_TYPE_MEMORY, SCOREP_RMA_SYNC_TYPE_NOTIFY_IN,
 * SCOREP_RMA_SYNC_TYPE_NOTIFY_OUT).
 */
void
SCOREP_RmaSync( SCOREP_RmaWindowHandle windowHandle,
                uint32_t               remote,
                SCOREP_RmaSyncType     syncType );


/**
 * This record marks the synchronization of a sub-group of the locations
 * associated with the given memory window. It needs to be recorded for
 * all participants.
 *
 * @param syncLevel Synchronization level.
 *
 * @param windowHandle Memory window.
 *
 * @param group Group of participating processes or threads.
 */
void
SCOREP_RmaGroupSync( SCOREP_RmaSyncLevel    syncLevel,
                     SCOREP_RmaWindowHandle windowHandle,
                     SCOREP_GroupHandle     groupHandle );


/**
 * The SCOREP_RmaWaitChange event marks a synchronization point that
 * blocks until a remote operation modifies a given memory field. This
 * event marks the beginning of the waiting period. The memory field in
 * question is part of the specified window.
 *
 * @param windowHandle Memory window.
 */
void
SCOREP_RmaWaitChange( SCOREP_RmaWindowHandle windowHandle );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
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
 */
void
SCOREP_RmaPut( SCOREP_RmaWindowHandle windowHandle,
               uint32_t               remote,
               uint64_t               bytes,
               uint64_t               matchingId );


/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
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
 */
void
SCOREP_RmaGet( SCOREP_RmaWindowHandle windowHandle,
               uint32_t               remote,
               uint64_t               bytes,
               uint64_t               matchingId );


/**
 * The atomic RMA operations are similar to the get and put operations.
 * As an additional field they provide the type of operation. Depending
 * on the type, data may be received, sent, or both, therefore, the
 * sizes are specified separately. Matching the local and optionally
 * remote completion works the same way as for get and put operations.
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
void
SCOREP_RmaAtomic( SCOREP_RmaWindowHandle windowHandle,
                  uint32_t               remote,
                  SCOREP_RmaAtomicType   type,
                  uint64_t               bytesSent,
                  uint64_t               bytesReceived,
                  uint64_t               matchingId );


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
SCOREP_RmaOpCompleteBlocking( SCOREP_RmaWindowHandle windowHandle,
                              uint64_t               matchingId );


void
SCOREP_RmaOpCompleteNonBlocking( SCOREP_RmaWindowHandle windowHandle,
                                 uint64_t               matchingId );

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
SCOREP_RmaOpTest( SCOREP_RmaWindowHandle windowHandle,
                  uint64_t               matchingId );


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
SCOREP_RmaOpCompleteRemote( SCOREP_RmaWindowHandle windowHandle,
                            uint64_t               matchingId );



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
SCOREP_Location_RmaWinCreate( SCOREP_Location*       location,
                              uint64_t               timestamp,
                              SCOREP_RmaWindowHandle windowHandle );


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
SCOREP_Location_RmaWinDestroy( SCOREP_Location*       location,
                               uint64_t               timestamp,
                               SCOREP_RmaWindowHandle windowHandle );


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
SCOREP_Location_RmaPut( SCOREP_Location*       location,
                        uint64_t               timestamp,
                        SCOREP_RmaWindowHandle windowHandle,
                        uint32_t               remote,
                        uint64_t               bytes,
                        uint64_t               matchingId );


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
SCOREP_Location_RmaGet( SCOREP_Location*       location,
                        uint64_t               timestamp,
                        SCOREP_RmaWindowHandle windowHandle,
                        uint32_t               remote,
                        uint64_t               bytes,
                        uint64_t               matchingId );


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
SCOREP_Location_RmaOpCompleteBlocking( SCOREP_Location*       location,
                                       uint64_t               timestamp,
                                       SCOREP_RmaWindowHandle windowHandle,
                                       uint64_t               matchingId );


/** @brief Records an IoCreateHandle event.
 *
 * An IoCreateHandle record marks the creation of a new file handle
 * that will be used by subsequent I/O operations.
 *
 * @param handle        The I/O handle which will be activated by this record.
 * @param mode          Access mode (e.g., read, write).
 * @param creationFlags Additional flags set while handle creation (e.g., tmpfile).
 * @param statusFlags   Additional status flags which can be changed during lifetime (e.g., close-on-exec).
 *
 */
void
SCOREP_IoCreateHandle( SCOREP_IoHandleHandle handle,
                       SCOREP_IoAccessMode   mode,
                       SCOREP_IoCreationFlag creationFlags,
                       SCOREP_IoStatusFlag   statusFlags );


/** @brief Records an IoDestroyHandle event.
 *
 * An IoDestroyHandle record marks the end of a I/O handle's lifetime.
 *
 * @param handle        I/O handle.
 *
 */
void
SCOREP_IoDestroyHandle( SCOREP_IoHandleHandle handle );


/** @brief Records an IoDuplicateHandle event.
 *
 * An IoDuplicateHandle record marks the duplication of an already
 * existing file handle.
 *
 * @param oldHandle     Original I/O handle.
 * @param newHandle     New I/O handle.
 * @param statusFlags   Additional status flags which can be changed during lifetime (e.g., close-on-exec).
 *
 */
void
SCOREP_IoDuplicateHandle( SCOREP_IoHandleHandle oldHandle,
                          SCOREP_IoHandleHandle newHandle,
                          SCOREP_IoStatusFlag   statusFlags );


/** @brief Records an IoSeek event.
 *
 * An IoSeek record marks a change of the position, e.g., within a file.
 *
 * @param handle        I/O handle.
 * @param offsetRequest Requested offset.
 * @param whence        Options for repositioning a offset with I/O seek operations.
 * @param offsetResult  Resulting offset, e.g., within the file relative to the beginning of the file.
 *
 */
void
SCOREP_IoSeek( SCOREP_IoHandleHandle handle,
               int64_t               offsetRequest,
               SCOREP_IoSeekOption   whence,
               uint64_t              offsetResult );


/** @brief Records an IoChangeFlags event.
 *
 * An IoChangeFlags record marks changes to the flags associated with an I/O handle.
 *
 * @param handle        I/O handle.
 * @param statusFlags   Additional status flags which can be changed during lifetime (e.g., close-on-exec).
 *
 */
void
SCOREP_IoChangeStatusFlags( SCOREP_IoHandleHandle handle,
                            SCOREP_IoStatusFlag   statusFlags );


/** @brief Records an IoDeleteFile event.
 *
 * An IoDeleteFile record marks the deletion of an I/O file.
 *
 * @param ioParadigm    Specification of the I/O paradigm.
 * @param ioFile        File identifier.
 *
 */
void
SCOREP_IoDeleteFile( SCOREP_IoParadigmType ioParadigm,
                     SCOREP_IoFileHandle   ioFile );


/** @brief Records an IoOperationBegin event.
 *
 * An IoOperationBegin record marks the begin of a file operation (read, write etc.).
 *
 * @param handle             I/O handle.
 * @param mode               Mode of a file operation (e.g., read or write).
 * @param operationFlags     Flags indicating specific semantics of the operation (e.g., non-blocking).
 * @param bytesRequest       Requested bytes to write/read.
 * @param matchingId         Identifier, used for correlating associated event records of an I/O operation.
 *
 */
void
SCOREP_IoOperationBegin( SCOREP_IoHandleHandle  handle,
                         SCOREP_IoOperationMode mode,
                         SCOREP_IoOperationFlag operationFlags,
                         uint64_t               bytesRequest,
                         uint64_t               matchingId );


/** @brief Records a IoOperationTest event.
 *
 * An IoOperationTest record marks the an unsuccessful test whether
 * an I/O operation has already finished.
 *
 * @param handle             I/O handle.
 * @param matchingId         Identifier, used for correlating associated event records of an I/O operation.
 */
void
SCOREP_IoOperationTest( SCOREP_IoHandleHandle handle,
                        uint64_t              matchingId );


/** @brief Records an IoOperationIssued event.
 *
 * An IoOperationIssued record marks the successful initiation of a
 * non-blocking file operation (read, write etc.).
 *
 * @param handle            I/O handle.
 * @param matchingId        Identifier, used for correlating associated event records of an I/O operation.
 */
void
SCOREP_IoOperationIssued( SCOREP_IoHandleHandle handle,
                          uint64_t              matchingId );



/** @brief Records an IoOperationComplete event.
 *
 * An IoOperationComplete record marks the end of a file operation
 * (read, write etc.). It keeps the necessary information for this
 * event: file, number of transferred bytes.
 *
 * @param handle            I/O handle.
 * @param mode              Mode of a file operation (e.g., read or write); same as in IoOperationBegin.
 * @param bytesResult       Number of transferred bytes (interpretation depends on operation).
 * @param matchingId        Identifier, used for correlating associated event records of an I/O operation.
 */
void
SCOREP_IoOperationComplete( SCOREP_IoHandleHandle  handle,
                            SCOREP_IoOperationMode mode,
                            uint64_t               bytesResult,
                            uint64_t               matchingId );


/** @brief Records an IoOperationCancelled event.
 *
 * An IoOperationCancelled record marks the successful cancellation of a
 * non-blocking file operation (read, write etc.).
 *
 * @param handle            I/O handle.
 * @param matchingId        Identifier, used for correlating associated event records of an I/O operation.
 */
void
SCOREP_IoOperationCancelled( SCOREP_IoHandleHandle handle,
                             uint64_t              matchingId );


/** @brief Records an IoAcquireLock event.
 *
 * Process the acquisition of an I/O lock (e.g., locking access to a file).
 *
 * @param handle            I/O handle.
 * @param lockType          Type of lock (shared vs. exclusive).
 */
void
SCOREP_IoAcquireLock( SCOREP_IoHandleHandle handle,
                      SCOREP_LockType       lockType );


/** @brief Records an IoReleaseLock event.
 *
 * Process the release of an I/O lock (e.g., release the lock to access a file).
 *
 * @param handle            I/O handle.
 * @param lockType          Type of lock (shared vs. exclusive).
 */
void
SCOREP_IoReleaseLock( SCOREP_IoHandleHandle handle,
                      SCOREP_LockType       lockType );


/** @brief Records an IoTryLock event.
 *
 * Process the attempt to acquire an I/O lock (e.g., locking access to a file).
 * However, this attempt was not succesfully and the lock was not granted.
 *
 * @param handle            I/O handle.
 * @param lockType          Type of lock (shared vs. exclusive).
 */
void
SCOREP_IoTryLock( SCOREP_IoHandleHandle handle,
                  SCOREP_LockType       lockType );


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
SCOREP_ThreadAcquireLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder );


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
SCOREP_ThreadReleaseLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder );


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


void
SCOREP_TrackAlloc( uint64_t addrAllocated,
                   size_t   bytesAllocated,
                   void*    substrateData[],
                   size_t   bytesAllocatedMetric,
                   size_t   bytesAllocatedProcess );


void
SCOREP_TrackRealloc( uint64_t oldAddr,
                     size_t   oldBytesAllocated,
                     void*    oldSubstrateData[],
                     uint64_t newAddr,
                     size_t   newBytesAllocated,
                     void*    newSubstrateData[],
                     size_t   bytesAllocatedMetric,
                     size_t   bytesAllocatedProcess );


void
SCOREP_TrackFree( uint64_t addrFreed,
                  size_t   bytesFreed,
                  void*    substrateData[],
                  size_t   bytesAllocatedMetric,
                  size_t   bytesAllocatedProcess );


void
SCOREP_LeakedMemory( uint64_t addrLeaked,
                     size_t   bytesLeaked,
                     void*    substrateData[] );


/**
 * Returns the time stamp of the last triggered event on the current location.
 *
 */
uint64_t
SCOREP_GetLastTimeStamp( void );

/*@}*/

#endif /* SCOREP_EVENTS_H */
