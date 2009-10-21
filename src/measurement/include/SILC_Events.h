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
 * After defining regions or other entities the adapter may call event functions. Most prominent are the region enter and exit functions which trigger the callpath handling and the metrics calculation.

   The main difference between VampirTrace and Scalasca regarding the event interface is the timestamping. Where Scalasca takes the timestamp in the measurement system, VampirTrace requires every adapter to do this. The Scalasca way introduces some overhead if additional timings are done on events that are logically at once. Prominent examples are MPI_Send/Recv where the MPI adapter obtain a timestamp on esd_enter and on esd_mpi_send although the are logically at once. It turns out that this extra timestamp isn't really necessary for Scalasca's analysis (maybe it was at some time). But certainly there may be events that are associated with the previous enter or following exit timestamp. FZJ is of the opinion that timestamping is the measurements job. If timestamping is done inside the measurement system only, we may need to add an additional parameter to the events in question to indicate to which timestamp they belong.


   The POMP adapter of VT and Scalasca have commonalities but also differ and I can't tell why. The *_omp_for/join functions differ a lot. There are special interface functions for OMP locking in Scalasca that can be filtered out and no equivalent in VT. The VT functions vt_omp_parallel_begin and vt_omp_parallel_end are called from the POMP adapter functions POMP_Parallel_begin/end. Scalasca calls esd_enter on POMP_Parallel_begin and esd_omp_collexit on POMP_Parallel_end here. Another commonality is the lack of documentation ...
 *
 * - Timestamping is done in the measurement system. The adapters need not to
     provide timestamps.
 *
 * - Nesting is user's responsibility
 *
 *
 * @todo MPI non-blocking, Milestone 2
 * @todo MPI RMA, Milestone 2
 * @todo MPI I/O, Milestone 2
 * @todo Interface for additional attributes, Milestone 3

 */
/*@{*/


/**
 * Generate an enter event in the measurement system.
 *
 * @param regionHandle
 */
void
SILC_EnterRegion
(
    SILC_RegionHandle regionHandle
);


/**
 * Generate an exit event in the measurement system.
 *
 * @param regionHandle
 */
void
SILC_ExitRegion
(
    SILC_RegionHandle regionHandle
);


/**
 * Generate an mpi send event in the measurement system.
 *
 * @param globalDestinationRank
 * @param communicatorHandle
 * @param tag
 * @param bytesSent
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
 * @param globalSourceRank
 * @param communicatorHandle
 * @param tag
 * @param bytesReceived
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
 * @param matchingId
 * @param regionHandle
 * @param communicatorHandle
 *
 * @param globalRootRank root rank in collective operation, or
 * SILC_INVALID_ROOT_RANK
 *
 * @param bytesSent
 * @param bytesReceived
 *
 * @todo what is the matchingId?
 */
void
SILC_MpiCollective
(
    uint32_t                   matchingId,
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
 */
void
SILC_TriggerParameterString
(
    SILC_ParameterHandle parameterHandle,
    const char*          value
);

/*@}*/

#endif /* SILC_EVENTS_H */
