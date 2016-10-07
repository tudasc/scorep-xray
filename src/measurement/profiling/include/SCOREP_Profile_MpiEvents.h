/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_MPIEVENTS_H
#define SCOREP_PROFILE_MPIEVENTS_H

/**
 * @file
 *
 * @brief   Declaration of MPI profiling events.
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_Location.h>
#include <SCOREP_DefinitionHandles.h>

/**
 * Initializes MPI Event specific data. Is called from MPI Initialize
 */
void
SCOREP_Profile_InitializeMpp( void );

/**
 * Process an MPI send event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param timestamp          Unused.
 * @param destinationRank    Unused.
 * @param communicatorHandle Unused.
 * @param tag                Unused.
 * @param bytesSent          Number of sent bytes.
 */
void
SCOREP_Profile_MpiSend( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent );

/**
 * Process an MPI receive event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param timestamp          Unused.
 * @param sourceRank         Unused.
 * @param communicatorHandle Unused.
 * @param tag                Unused.
 * @param bytesReceived      Number of received bytes.
 */
void
SCOREP_Profile_MpiRecv( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesReceived );

/**
 * Process a collective MPI communication event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param timestamp          Unused.
 * @param communicatorHandle Unused.
 * @param rootRank           Unused.
 * @param collectiveType     Unused.
 * @param bytesSent          Number of sent bytes.
 * @param bytesReceived      Number of received bytes.
 */
void
SCOREP_Profile_CollectiveEnd( SCOREP_Location*                 location,
                              uint64_t                         timestamp,
                              SCOREP_InterimCommunicatorHandle communicatorHandle,
                              SCOREP_MpiRank                   rootRank,
                              SCOREP_CollectiveType            collectiveType,
                              uint64_t                         bytesSent,
                              uint64_t                         bytesReceived );

/**
 * Process a collective RMA communication event in the profile.
 * @param location      Location object for the thread where the event occurred.
 * @param timestamp     Unused.
 * @param collectiveOp  Unused.
 * @param syncLevel     Synchronization level.
 * @param windowHandle  Unused.
 * @param root          Unused.
 * @param bytesSent     Number of bytes sent.
 * @param bytesReceived Number of bytes received.
 */
void
SCOREP_Profile_RmaCollectiveEnd( SCOREP_Location*       location,
                                 uint64_t               timestamp,
                                 SCOREP_CollectiveType  collectiveOp,
                                 SCOREP_RmaSyncLevel    syncLevel,
                                 SCOREP_RmaWindowHandle windowHandle,
                                 uint32_t               root,
                                 uint64_t               bytesSent,
                                 uint64_t               bytesReceived );


/**
 * Process an MPI send event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param timestamp          Unused.
 * @param destinationRank    Unused.
 * @param communicatorHandle Unused.
 * @param tag                Unused.
 * @param bytesSent          Number of sent bytes.
 * @param requestId          Unused.
 */
void
SCOREP_Profile_MpiIsend( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   destinationRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesSent,
                         SCOREP_MpiRequestId              requestId );


/**
 * Process an MPI receive event in the profile.
 * @param location           Location object for the thread where the event occurred.
 * @param timestamp          Unused.
 * @param sourceRank         Unused.
 * @param communicatorHandle Unused.
 * @param tag                Unused.
 * @param bytesReceived      Number of received bytes.
 * @param requestId          Unused.
 */
void
SCOREP_Profile_MpiIrecv( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   sourceRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesReceived,
                         SCOREP_MpiRequestId              requestId );


/**
 * This record marks a simple pairwise synchronization.
 *
 * @param location     Location object for the thread where the event occurred.
 * @param timestamp    Unused.
 * @param windowHandle Unused.
 * @param remote       Unused.
 * @param syncType     Synchronization level (e.g. SCOREP_RMA_SYNC_LEVEL_NONE,
 *                     SCOREP_RMA_SYNC_LEVEL_PROCESS, SCOREP_RMA_SYNC_LEVEL_MEMORY).
 */
void
SCOREP_Profile_RmaSync( SCOREP_Location*       location,
                        uint64_t               timestamp,
                        SCOREP_RmaWindowHandle windowHandle,
                        uint32_t               remote,
                        SCOREP_RmaSyncType     syncType );


/**
 * This record marks the synchronization of a sub-group of the locations
 * associated with the given memory window. It needs to be recorded for
 * all participants.
 *
 * @param location     Location object for the thread where the event occurred.
 * @param timestamp    Timestamp when the event occurred.
 * @param syncLevel    Synchronization level.
 * @param windowHandle Unused.
 * @param groupHandle  Unused.
 */
void
SCOREP_Profile_RmaGroupSync( SCOREP_Location*       location,
                             uint64_t               timestamp,
                             SCOREP_RmaSyncLevel    syncLevel,
                             SCOREP_RmaWindowHandle windowHandle,
                             SCOREP_GroupHandle     groupHandle );

/**
 * The get and put operations access remote memory addresses. The
 * corresponding get and put records mark when they are issued. The
 * actual start and the completion may happen later.
 *
 * @param location     Location object for the thread where the event occurred.
 * @param timestamp    Unused.
 * @param windowHandle Unused.
 * @param remote       Unused.
 * @param bytes        Number of bytes transferred.
 * @param matchingId   Unused.
 *
 * @note The matching number allows to reference the point of completion
 * of the operation. It will reappear in a completion record on the same
 * location.
 *
 */
void
SCOREP_Profile_RmaPut( SCOREP_Location*       location,
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
 * @param location     Location object for the thread where the event occurred.
 * @param timestamp    Unused.
 * @param windowHandle Unused.
 * @param remote       Unused.
 * @param bytes        Number of bytes transferred.
 * @param matchingId   Unused.
 *
 * @note The matching number allows to reference the point of completion
 * of the operation. It will reappear in a completion record on the same
 * location.
 *
 */
void
SCOREP_Profile_RmaGet( SCOREP_Location*       location,
                       uint64_t               timestamp,
                       SCOREP_RmaWindowHandle windowHandle,
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
 * @param location      Location object for the thread where the event occurred.
 * @param timestamp     Timestamp when the event occurred.
 * @param windowHandle  Unused.
 * @param remote        Unused.
 * @param type          Type of atomic operation (see @a SCOREP_RmaAtomicType).
 * @param bytesSent     Number of bytes transferred to remote target.
 * @param bytesReceived Number of bytes transferred from remote target.
 * @param matchingId    Unused.
 *
 */
void
SCOREP_Profile_RmaAtomic( SCOREP_Location*       location,
                          uint64_t               timestamp,
                          SCOREP_RmaWindowHandle windowHandle,
                          uint32_t               remote,
                          SCOREP_RmaAtomicType   type,
                          uint64_t               bytesSent,
                          uint64_t               bytesReceived,
                          uint64_t               matchingId );




#endif /* SCOREP_PROFILE_MPIEVENTS_H */
