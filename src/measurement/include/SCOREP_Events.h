/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SCOREP_EVENTS_H
#define SCOREP_EVENTS_H


/**
 * @file        SCOREP_Events.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
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
SCOREP_EnterRegion
(
    SCOREP_RegionHandle regionHandle
);





/**
 * Process a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the exit event.
 */
void
SCOREP_ExitRegion
(
    SCOREP_RegionHandle regionHandle
);




/**
 * Generate a rewind region enter event in the measurement system.
 *
 * @param regionHandle The corresponding rewind region for the enter event.
 */
void
SCOREP_EnterRewindRegion
(
    SCOREP_RegionHandle regionHandle
);

/**
 * Generate a rewind region exit event in the measurement system.
 *
 * @param regionHandle The corresponding rewind region for the exit event.
 * @param value A boolean parameter, whether the trace buffer should be
 *              rewinded (value = true) or not (value = false).
 */
void
SCOREP_ExitRewindRegion
(
    SCOREP_RegionHandle regionHandle,
    bool                value
);


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
SCOREP_Location_ExitRegion
(
    SCOREP_Location*    location,
    uint64_t            timestamp,
    SCOREP_RegionHandle regionHandle
);




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
SCOREP_Location_EnterRegion
(
    SCOREP_Location*    location,
    uint64_t            timestamp,
    SCOREP_RegionHandle regionHandle
);




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
 * @see SCOREP_DefineLocalMPICommunicator()
 */
void
SCOREP_MpiSend( SCOREP_MpiRank                    destinationRank,
                SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                uint32_t                          tag,
                uint64_t                          bytesSent );


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
SCOREP_MpiRecv( SCOREP_MpiRank                    sourceRank,
                SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                uint32_t                          tag,
                uint64_t                          bytesReceived );


/**
 * Process an mpi collective begin event in the measurement system.
 *
 * Records also an enter event into the region @regionHandle.
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
 * Records also an leave event out of the region @regionHandle.
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
SCOREP_MpiCollectiveEnd( SCOREP_RegionHandle               regionHandle,
                         SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                         SCOREP_MpiRank                    rootRank,
                         SCOREP_MpiCollectiveType          collectiveType,
                         uint64_t                          bytesSent,
                         uint64_t                          bytesReceived );

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
 * @see SCOREP_DefineLocalMPICommunicator()
 */
void
SCOREP_MpiIsend( SCOREP_MpiRank                    destinationRank,
                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                 uint32_t                          tag,
                 uint64_t                          bytesSent,
                 SCOREP_MpiRequestId               requestId );


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
SCOREP_MpiIrecv( SCOREP_MpiRank                    sourceRank,
                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                 uint32_t                          tag,
                 uint64_t                          bytesReceived,
                 SCOREP_MpiRequestId               requestId );


/**
 * Notify the measurement system about the creation of a fork-join
 * parallel execution with at max @a nRequestedThreads new
 * threads. This function needs to be triggered for every thread
 * creation in a fork-join model, e.g., #pragma omp parallel in OpenMP
 * (for create-wait models see SCOREP_ThreadCreate()).
 * SCOREP_ThreadFork() needs to be called outside the parallel
 * execution from the thread creating the parallel region.
 *
 * @param nRequestedThreads Upper bound of threads that comprise the
 * parallel region to be created.
 *
 * @param model One of the predefined threading models.
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
SCOREP_ThreadFork( uint32_t           nRequestedThreads,
                   SCOREP_ThreadModel model );


/**
 * Notify the measurement system about the completion of a fork-join
 * parallel execution. The parallel execution was started by a call to
 * SCOREP_ThreadFork() that returned the @a forkSequenceCount that
 * needs to be provided to this function.
 *
 * @param forkSequenceCount The fork sequence count returned by the
 * corresponding SCOREP_ThreadFork() or
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the latter,
 * the forkSequenceCount should be maintained in the model-specific
 * implementation, see e.g., the OpenMP implementation of
 * SCOREP_Thread_OnJoin().
 *
 * @param model One of the predefined threading models.
 *
 * @note See the notes to SCOREP_ThreadFork().
 */
void
SCOREP_ThreadJoin( uint32_t           forkSequenceCount,
                   SCOREP_ThreadModel model );


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
 * @param forkSequenceCount The fork sequence count returned by the
 * corresponding SCOREP_ThreadCreate() or
 * SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the latter,
 * the forkSequenceCount should be maintained in the model-specific
 * implementation.
 *
 * @param model One of the predefined threading models.
 */
void
SCOREP_ThreadWait( uint32_t           forkSequenceCount,
                   SCOREP_ThreadModel model );


/**
 * Notify the measurement system about the begin of a parallel
 * execution on a thread created by either SCOREP_ThreadFork() or
 * SCOREP_ThreadCreate(). In case of SCOREP_ThreadFork() all created
 * threads including the master must call SCOREP_ThreadTeamBegin().
 *
 * @param forkSequenceCount The forkSequenceCount returned by the
 * corresponding SCOREP_ThreadFork() or SCOREP_ThreadCreate() call
 * or SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT. If you pass the
 * latter, the forkSequenceCount should be maintained in the
 * model-specific implementation.
 *
 * @threadId Id within the team of threads that constitute the
 * parallel region.
 *
 * @param model One of the predefined threading models.
 *
 * @note The end of the parallel execution will be signalled by a call
 * to SCOREP_ThreadEnd().
 *
 * @note Per convention and as there is no parallelism for the initial
 * thread we don't call SCOREP_ThreadTeamBegin() and SCOREP_ThreadEnd()
 * for the initial thread.
 */
void
SCOREP_ThreadTeamBegin( uint32_t           forkSequenceCount,
                        uint32_t           threadId,
                        SCOREP_ThreadModel model );


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
SCOREP_ThreadEnd( uint32_t           forkSequenceCount,
                  SCOREP_ThreadModel model );


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
SCOREP_ThreadAcquireLock( uint32_t           lockId,
                          uint32_t           acquisitionOrder,
                          SCOREP_ThreadModel model );


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
SCOREP_ThreadReleaseLock( uint32_t           lockId,
                          uint32_t           acquisitionOrder,
                          SCOREP_ThreadModel model );


/**
 *
 *
 * @param regionHandle
 *
 * @chistian Wasn't this planned to be removed?
 */
void
SCOREP_ExitRegionOnException
(
    SCOREP_RegionHandle regionHandle
);


/**
 * Process a task create event in the measurement system.
 *
 * @param taskId Id of the created task.
 */
void
SCOREP_ThreadTaskCreate( uint64_t           taskId,
                         SCOREP_ThreadModel model );


/**
 * Process a task switch event in the measurement system.
 *
 * @param taskId Id of the task the runtime switched to.
 */
void
SCOREP_ThreadTaskSwitch( uint64_t           taskId,
                         SCOREP_ThreadModel model );


/**
 * Process a task begin event in the measurement system.
 *
 * @param regionHandle region handle of the task region.
 * @param taskId Id of the starting task.
 */
void
SCOREP_ThreadTaskBegin( SCOREP_RegionHandle regionHandle,
                        uint64_t            taskId,
                        SCOREP_ThreadModel  model );

/**
 * Process a task end event in the measurement system.
 *
 * @param regionHandle region handle of the task region.
 * @param taskId Id of the completed task.
 */
void
SCOREP_ThreadTaskEnd( SCOREP_RegionHandle regionHandle,
                      uint64_t            taskId,
                      SCOREP_ThreadModel  model );


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterInt64
(
    SCOREP_SamplingSetHandle counterHandle,
    int64_t                  value
);


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterUint64
(
    SCOREP_SamplingSetHandle counterHandle,
    uint64_t                 value
);


/**
 *
 *
 * @param counterHandle
 * @param value
 */
void
SCOREP_TriggerCounterDouble
(
    SCOREP_SamplingSetHandle counterHandle,
    double                   value
);


/**
 *
 *
 * @param markerHandle
 *
 * @planned To be implemented in milestone 2
 */
void
SCOREP_TriggerMarker
(
    SCOREP_MarkerHandle markerHandle
);


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterInt64
(
    SCOREP_ParameterHandle parameterHandle,
    int64_t                value
);


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterUint64
(
    SCOREP_ParameterHandle parameterHandle,
    uint64_t               value
);


/**
 *
 *
 * @param parameterHandle
 * @param value
 */
void
SCOREP_TriggerParameterString
(
    SCOREP_ParameterHandle parameterHandle,
    const char*            value
);


/**
 * Returns the time stamp of the last triggered event
 *
 */
uint64_t
SCOREP_GetLastTimeStamp
(
);

/*@}*/


#endif /* SCOREP_EVENTS_H */
