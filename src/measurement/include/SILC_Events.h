#ifndef SILC_EVENTS_H
#define SILC_EVENTS_H

/**
 * @file        SILC_Events.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */

#include "SILC_Types.h"

/**
 * @defgroup SILC_Events SILC Events
 *
 * - After defining regions or other entities the adapter may call event
     functions. Most prominent are the region enter (SILC_EnterRegion()) and
     exit (SILC_ExitRegion()) functions which trigger the callpath handling,
     the metrics calculation and the trace writing. The user is reposible for
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
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SILC_EnterRegion
(
    SILC_RegionHandle regionHandle
);


/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the exit event.
 */
void
SILC_ExitRegion
(
    SILC_RegionHandle regionHandle
);


/**
 * Generate an mpi send event in the measurement system.
 *
 * @param globalDestinationRank The MPI destination rank in MPI_COMM_WORLD. If
 * your communicator is not MPI_COMM_WORLD, you need to convert your local
 * rank to the corresponding rank in MPI_COMM_WORLD.
 *
 * @param communicatorHandle The previosly defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @note @a globalDestinationRank and @a communicatorHandle may change in
 * future versions to @a localRank and @a groupHandle for performance
 * reasons. Querying the global rank is quite expensive if you are not in
 * MPI_COMM_WORLD.
 *
 * @see SILC_DefineMPICommunicator()
 */
void
SILC_MpiSend
(
    int32_t                    globalDestinationRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesSent
);


/**
 * Generate an mpi send event in the measurement system.
 *
 * @param globalSourceRank The MPI source rank in MPI_COMM_WORLD. If your
 * communicator is not MPI_COMM_WORLD, you need to convert your local rank to
 * the corresponding rank in MPI_COMM_WORLD.
 *
 * @param communicatorHandle The previosly defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param tag The MPI tag used in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 *
 * @note @a globalSourceRank and @a communicatorHandle may change in future
 * versions to @a localRank and @a groupHandle for performance
 * reasons. Querying the global rank is quite expensive if you are not in
 * MPI_COMM_WORLD.
 *
 * @todo Do measurements to compare the global/local rank performance at large
 * scale.
 */
void
SILC_MpiRecv
(
    int32_t                    globalSourceRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesReceived
);


/**
 * Generate an mpi collective event in the measurement system.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @param communicatorHandle The previosly defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param globalRootRank Root rank of the collective operation in
 * MPI_COMM_WORLD, or SILC_INVALID_ROOT_RANK.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 */
void
SILC_MpiCollective
(
    SILC_RegionHandle          regionHandle,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    globalRootRank,
    int32_t                    bytesSent,
    int32_t                    bytesReceived
);


/**
 *
 *
 * @param regionHandle
 */
void
SILC_OmpFork
(
    SILC_RegionHandle regionHandle
);


/**
 *
 *
 * @param regionHandle
 */
void
SILC_OmpJoin
(
    SILC_RegionHandle regionHandle
);

/**
 *
 *
 * @param lockId
 */
void
SILC_OmpAcquireLock
(
    uint32_t lockId
);


/**
 *
 *
 * @param lockId
 */
void
SILC_OmpReleaseLock
(
    uint32_t lockId
);


/**
 *
 *
 * @param regionHandle
 */
void
SILC_ExitRegionOnException
(
    SILC_RegionHandle regionHandle
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
SILC_TriggerCounterInt64
(
    SILC_CounterHandle counterHandle,
    int64_t            value
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
SILC_TriggerCounterDouble
(
    SILC_CounterHandle counterHandle,
    double             value
);


/**
 *
 *
 * @param markerHandle
 *
 * @planned To be implemented in milestone 2
 */
void
SILC_TriggerMarker
(
    SILC_MarkerHandle markerHandle
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
SILC_TriggerParameterInt64
(
    SILC_ParameterHandle parameterHandle,
    int64_t              value
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
SILC_TriggerParameterDouble
(
    SILC_ParameterHandle parameterHandle,
    double               value
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
SILC_TriggerParameterString
(
    SILC_ParameterHandle parameterHandle,
    const char*          value
);

/*@}*/

#endif /* SILC_EVENTS_H */
