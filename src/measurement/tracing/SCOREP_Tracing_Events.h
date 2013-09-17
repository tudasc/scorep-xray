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


#ifndef SCOREP_TRACING_EVENTS_H
#define SCOREP_TRACING_EVENTS_H


/**
 * @file
 *
 * @brief Event consumption functions for tracing.
 */


#include <stdint.h>
#include <stdbool.h>


#include <scorep_location.h>

void
SCOREP_Tracing_MeasurementOnOff( SCOREP_Location* location,
                                 uint64_t         timestamp,
                                 bool             on );


void
SCOREP_Tracing_Metric( SCOREP_Location*         location,
                       uint64_t                 timestamp,
                       SCOREP_SamplingSetHandle samplingSet,
                       const uint64_t*          metricValues );


void
SCOREP_Tracing_Enter( SCOREP_Location*    location,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle regionHandle );


void
SCOREP_Tracing_Leave( SCOREP_Location*    location,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle regionHandle );


void
SCOREP_Tracing_MpiSend( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent );


void
SCOREP_Tracing_MpiRecv( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent );


void
SCOREP_Tracing_MpiCollectiveBegin( SCOREP_Location* location,
                                   uint64_t         timestamp );


void
SCOREP_Tracing_MpiCollectiveEnd( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_InterimCommunicatorHandle communicatorHandle,
                                 SCOREP_MpiRank                   rootRank,
                                 SCOREP_MpiCollectiveType         collectiveType,
                                 uint64_t                         bytesSent,
                                 uint64_t                         bytesReceived );


void
SCOREP_Tracing_MpiIsendComplete( SCOREP_Location*    location,
                                 uint64_t            timestamp,
                                 SCOREP_MpiRequestId requestId );


void
SCOREP_Tracing_MpiIrecvRequest( SCOREP_Location*    location,
                                uint64_t            timestamp,
                                SCOREP_MpiRequestId requestId );


void
SCOREP_Tracing_MpiRequestTested( SCOREP_Location*    location,
                                 uint64_t            timestamp,
                                 SCOREP_MpiRequestId requestId );


void
SCOREP_Tracing_MpiRequestCancelled( SCOREP_Location*    location,
                                    uint64_t            timestamp,
                                    SCOREP_MpiRequestId requestId );


void
SCOREP_Tracing_MpiIsend(  SCOREP_Location*                 location,
                          uint64_t                         timestamp,
                          SCOREP_MpiRank                   destinationRank,
                          SCOREP_InterimCommunicatorHandle communicatorHandle,
                          uint32_t                         tag,
                          uint64_t                         bytesSent,
                          SCOREP_MpiRequestId              requestId );


void
SCOREP_Tracing_MpiIrecv( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   sourceRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesReceived,
                         SCOREP_MpiRequestId              requestId );


void
SCOREP_Tracing_RmaWinCreate( SCOREP_Location*              location,
                             uint64_t                      timestamp,
                             SCOREP_InterimRmaWindowHandle windowHandle );


void
SCOREP_Tracing_RmaWinDestroy( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle );


void
SCOREP_Tracing_RmaCollectiveBegin( SCOREP_Location* location,
                                   uint64_t         timestamp );


void
SCOREP_Tracing_RmaCollectiveEnd( SCOREP_Location*              location,
                                 uint64_t                      timestamp,
                                 SCOREP_MpiCollectiveType      collectiveOp,
                                 SCOREP_RmaSyncLevel           syncLevel,
                                 SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint32_t                      root,
                                 uint64_t                      bytesSent,
                                 uint64_t                      bytesReceived );


void
SCOREP_Tracing_RmaGroupSync( SCOREP_Location*              location,
                             uint64_t                      timestamp,
                             SCOREP_RmaSyncLevel           syncLevel,
                             SCOREP_InterimRmaWindowHandle windowHandle,
                             SCOREP_GroupHandle            groupHandle );


void
SCOREP_Tracing_RmaRequestLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId,
                               SCOREP_LockType               lockType );


void
SCOREP_Tracing_RmaAcquireLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId,
                               SCOREP_LockType               lockType );


void
SCOREP_Tracing_RmaTryLock( SCOREP_Location*              location,
                           uint64_t                      timestamp,
                           SCOREP_InterimRmaWindowHandle windowHandle,
                           uint32_t                      remote,
                           uint64_t                      lockId,
                           SCOREP_LockType               lockType );


void
SCOREP_Tracing_RmaReleaseLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId );


void
SCOREP_Tracing_RmaSync( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        SCOREP_RmaSyncType            syncType );


void
SCOREP_Tracing_RmaWaitChange( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle );


void
SCOREP_Tracing_RmaPut( SCOREP_Location*              location,
                       uint64_t                      timestamp,
                       SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      bytes,
                       uint64_t                      matchingId );


void
SCOREP_Tracing_RmaGet( SCOREP_Location*              location,
                       uint64_t                      timestamp,
                       SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      bytes,
                       uint64_t                      matchingId );


void
SCOREP_Tracing_RmaAtomic( SCOREP_Location*              location,
                          uint64_t                      timestamp,
                          SCOREP_InterimRmaWindowHandle windowHandle,
                          uint32_t                      remote,
                          SCOREP_RmaAtomicType          type,
                          uint64_t                      bytesSent,
                          uint64_t                      bytesReceived,
                          uint64_t                      matchingId );


void
SCOREP_Tracing_RmaOpCompleteBlocking( SCOREP_Location*              location,
                                      uint64_t                      timestamp,
                                      SCOREP_InterimRmaWindowHandle windowHandle,
                                      uint64_t                      matchingId );


void
SCOREP_Tracing_RmaOpCompleteNonBlocking( SCOREP_Location*              location,
                                         uint64_t                      timestamp,
                                         SCOREP_InterimRmaWindowHandle windowHandle,
                                         uint64_t                      matchingId );


void
SCOREP_Tracing_RmaOpTest( SCOREP_Location*              location,
                          uint64_t                      timestamp,
                          SCOREP_InterimRmaWindowHandle windowHandle,
                          uint64_t                      matchingId );


void
SCOREP_Tracing_RmaOpCompleteRemote( SCOREP_Location*              location,
                                    uint64_t                      timestamp,
                                    SCOREP_InterimRmaWindowHandle windowHandle,
                                    uint64_t                      matchingId );


void
SCOREP_Tracing_ThreadFork( SCOREP_Location*   location,
                           uint64_t           timestamp,
                           SCOREP_ThreadModel model,
                           uint32_t           nRequestedThreads,
                           uint32_t           forkSequenceCount );


void
SCOREP_Tracing_ThreadTeamBegin( SCOREP_Location*                 location,
                                uint64_t                         timestamp,
                                SCOREP_ThreadModel               model,
                                SCOREP_InterimCommunicatorHandle threadTeam );


void
SCOREP_Tracing_ThreadTeamEnd( SCOREP_Location*                 location,
                              uint64_t                         timestamp,
                              SCOREP_ThreadModel               model,
                              SCOREP_InterimCommunicatorHandle threadTeam );


void
SCOREP_Tracing_ThreadJoin( SCOREP_Location*   location,
                           uint64_t           timestamp,
                           SCOREP_ThreadModel model,
                           uint32_t           forkSequenceCount );


void
SCOREP_Tracing_ThreadAcquireLock( SCOREP_Location*   location,
                                  uint64_t           timestamp,
                                  SCOREP_ThreadModel model,
                                  uint32_t           lockId,
                                  uint32_t           acquisitionOrder );


void
SCOREP_Tracing_ThreadReleaseLock( SCOREP_Location*   location,
                                  uint64_t           timestamp,
                                  SCOREP_ThreadModel model,
                                  uint32_t           lockId,
                                  uint32_t           acquisitionOrder );


void
SCOREP_Tracing_ThreadTaskCreate( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_ThreadModel               model,
                                 SCOREP_InterimCommunicatorHandle threadTeam,
                                 uint32_t                         threadId,
                                 uint32_t                         generationNumber );


void
SCOREP_Tracing_ThreadTaskSwitch( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_ThreadModel               model,
                                 SCOREP_InterimCommunicatorHandle threadTeam,
                                 uint32_t                         threadId,
                                 uint32_t                         generationNumber );


void
SCOREP_Tracing_ThreadTaskComplete( SCOREP_Location*                 location,
                                   uint64_t                         timestamp,
                                   SCOREP_ThreadModel               model,
                                   SCOREP_InterimCommunicatorHandle threadTeam,
                                   uint32_t                         threadId,
                                   uint32_t                         generationNumber );


void
SCOREP_Tracing_ParameterInt64( SCOREP_Location*       location,
                               uint64_t               timestamp,
                               SCOREP_ParameterHandle parameterHandle,
                               int64_t                value );


void
SCOREP_Tracing_ParameterUint64( SCOREP_Location*       location,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle parameterHandle,
                                uint64_t               value );


void
SCOREP_Tracing_ParameterString( SCOREP_Location*       location,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle parameterHandle,
                                SCOREP_StringHandle    stringHandle );


void
SCOREP_Tracing_StoreRewindPoint( SCOREP_Location*    location,
                                 SCOREP_RegionHandle regionHandle,
                                 uint64_t            timestamp );


void
SCOREP_Tracing_ClearRewindPoint( SCOREP_Location* location,
                                 uint32_t         region_id );


void
SCOREP_Tracing_Rewind( SCOREP_Location* location,
                       uint32_t         region_id );


void
SCOREP_Tracing_ExitRewindRegion( SCOREP_Location*    location,
                                 SCOREP_RegionHandle regionHandle,
                                 uint64_t            leavetimestamp,
                                 bool                do_rewind );


size_t
SCOREP_Tracing_GetSamplingSetCacheSize( uint32_t numberOfMetrics );


void
SCOREP_Tracing_CacheSamplingSet( SCOREP_SamplingSetHandle samplingSet );


#endif /* SCOREP_TRACING_EVENTS_H */
