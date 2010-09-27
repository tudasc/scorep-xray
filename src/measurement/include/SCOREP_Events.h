/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
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
 * Generate a region enter event in the measurement system.
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
 * Generate a region exit event in the measurement system.
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
 * Generate an mpi send event in the measurement system.
 *
 * @param globalDestinationRank The MPI destination rank in MPI_COMM_WORLD. If
 * your communicator is not MPI_COMM_WORLD, you need to convert your local
 * rank to the corresponding rank in MPI_COMM_WORLD.
 *
 * @param communicatorHandle The previously defined handle belonging to the
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
 * @see SCOREP_DefineMPICommunicator()
 */
void
SCOREP_MpiSend
(
    uint32_t                     globalDestinationRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesSent
);


/**
 * Generate an mpi recv event in the measurement system.
 *
 * @param globalSourceRank The MPI source rank in MPI_COMM_WORLD. If your
 * communicator is not MPI_COMM_WORLD, you need to convert your local rank to
 * the corresponding rank in MPI_COMM_WORLD.
 *
 * @param communicatorHandle The previously defined handle belonging to the
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
SCOREP_MpiRecv
(
    uint32_t                     globalSourceRank,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     tag,
    uint64_t                     bytesReceived
);


/**
 * Generate an mpi collective event in the measurement system.
 *
 * @param regionHandle The region handle corresponding to the MPI function
 * that triggers this event.
 *
 * @param communicatorHandle The previously defined handle belonging to the
 * communicator that is used in this communication.
 *
 * @param globalRootRank Root rank of the collective operation in
 * MPI_COMM_WORLD, or SCOREP_INVALID_ROOT_RANK.
 *
 * @param bytesSent The number of bytes send in the communication.
 *
 * @param bytesReceived The number of bytes received in the communication.
 */
void
SCOREP_MpiCollective
(
    SCOREP_RegionHandle          regionHandle,
    SCOREP_MPICommunicatorHandle communicatorHandle,
    uint32_t                     globalRootRank,
    uint64_t                     bytesSent,
    uint64_t                     bytesReceived
);


/**
 * Generate an OpenMP fork event in the measurement system.
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
 * Generate an OpenMP join event in the measurement system.
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
 * Generate an OpenMP acquire lock event in the measurement system.
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
 * Generate an OpenMP release lock event in the measurement system.
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
