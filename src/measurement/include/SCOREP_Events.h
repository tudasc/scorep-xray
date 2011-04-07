/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
#include "SCOREP_DefinitionHandles.h"


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


// scorep_meets_tau_begin
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

// scorep_meets_tau_end


// scorep_meets_tau_begin
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

// scorep_meets_tau_end


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
 * @see SCOREP_DefineMPICommunicator()
 */
void
SCOREP_MpiSend
(
    SCOREP_MpiRank               destinationRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesSent
);


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
SCOREP_MpiRecv
(
    SCOREP_MpiRank               sourceRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesReceived
);


/**
 * Process an mpi collective begin event in the measurement system.
 *
 * Records also an enter event into the region @regionHandle.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param globalRootRank Root rank of the collective operation in
 * the communicator specified with @a communicatorHandle,
 * or SCOREP_INVALID_ROOT_RANK.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 *
 * @param matchingId A process unique sequence number to match the corresponding
 *                   SCOREP_MpiCollectiveEnd event
 *
 * @return The used timestamp for this event.
 */
uint64_t
SCOREP_MpiCollectiveBegin
(
    SCOREP_RegionHandle          regionHandle,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    SCOREP_MpiRank               rootRank,
    SCOREP_MpiCollectiveType     collectiveType,
    uint64_t                     bytesSent,
    uint64_t                     bytesReceived,
    uint64_t                     matchingId
);

/**
 * Process an mpi collective event in the measurement system.
 *
 * Records also an leave event out of the region @regionHandle.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @param matchingId A process unique sequence number to match the corresponding
 *                   SCOREP_MpiCollectiveBegin event
 */
void
SCOREP_MpiCollectiveEnd
(
    SCOREP_RegionHandle regionHandle,
    uint64_t            matchingId
);

/**
 * Processs a MPI send complete event in the measurement system.
 * @param requestId The request identifier, which specifies the request that was
 *                  completed.
 */
void
SCOREP_MpiSendComplete( SCOREP_MpiRequestId requestId );

/**
 * Processs a MPI receive request event in the measurement system.
 * @param requestId The request identifier, which specifies the new receive request.
 */
void
SCOREP_MpiRecvRequest( SCOREP_MpiRequestId requestId );

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
 * @see SCOREP_DefineMPICommunicator()
 */
void
SCOREP_MpiIsend
(
    SCOREP_MpiRank               destinationRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesSent,
    SCOREP_MpiRequestId          requestId
);


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
SCOREP_MpiIrecv
(
    SCOREP_MpiRank               sourceRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesReceived,
    SCOREP_MpiRequestId          requestId
);


/**
 * Process an OpenMP fork event in the measurement system.
 *
 * @param regionHandle The previous defined region handle which identifies the
 *                     the region into which this event forks.
 * @param nRequestedThreads An upper bound for the number of threads created by
                            this event.
 *
 * @see SCOREP_DefineRegion()
 */
void
SCOREP_OmpFork
(
    SCOREP_RegionHandle regionHandle,
    uint32_t            nRequestedThreads
);


/**
 * Process an OpenMP join event in the measurement system.
 *
 * @param regionHandle The previous defined region handle which identifies the
 *                     the region from which this event joins.
 *
 * @see SCOREP_DefineRegion()
 */
void
SCOREP_OmpJoin
(
    SCOREP_RegionHandle regionHandle
);


/**
 * Process an OpenMP acquire lock event in the measurement system.
 *
 * @param lockId A unique ID to identify the lock. Needs to be maintained by
 *               the caller.
 */
void
SCOREP_OmpAcquireLock
(
    uint32_t lockId
);


/**
 * Process an OpenMP release lock event in the measurement system.
 *
 * @param lockId A unique ID to identify the lock. Needs to be maintained by
 *               the caller.
 */
void
SCOREP_OmpReleaseLock
(
    uint32_t lockId
);


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
 *
 *
 * @param counterHandle
 * @param value
 *
 * @planned To be implemented in milestone 2
 */
void
SCOREP_TriggerCounterInt64
(
    SCOREP_CounterHandle counterHandle,
    int64_t              value
);


/**
 *
 *
 * @param counterHandle
 * @param value
 *
 * @planned To be implemented in milestone 2
 */
void
SCOREP_TriggerCounterDouble
(
    SCOREP_CounterHandle counterHandle,
    double               value
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
 *
 * @planned To be implemented in milestone 3
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
 *
 * @planned To be implemented in milestone 3
 */
void
SCOREP_TriggerParameterDouble
(
    SCOREP_ParameterHandle parameterHandle,
    double                 value
);


/**
 *
 *
 * @param parameterHandle
 * @param value
 *
 * @planned To be implemented in milestone 3
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
