/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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


#ifndef SCOREP_TRACING_EVENTS_H
#define SCOREP_TRACING_EVENTS_H


/**
 * @file        src/measurement/tracing/SCOREP_Tracing_Events.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 */


#include <scorep_thread.h>


void
SCOREP_Tracing_MeasurementOnOff( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 bool                        on );


void
SCOREP_Tracing_Metric( SCOREP_Thread_LocationData* location,
                       uint64_t                    timestamp,
                       SCOREP_SamplingSetHandle    samplingSet,
                       uint8_t                     numberOfMetrics,
                       const uint64_t*             metricValues );


void
SCOREP_Tracing_Enter( SCOREP_Thread_LocationData* location,
                      uint64_t                    timestamp,
                      SCOREP_RegionHandle         regionHandle );


void
SCOREP_Tracing_Leave( SCOREP_Thread_LocationData* location,
                      uint64_t                    timestamp,
                      SCOREP_RegionHandle         regionHandle );


void
SCOREP_Tracing_MpiSend( SCOREP_Thread_LocationData*       location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    destinationRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent );


void
SCOREP_Tracing_MpiRecv( SCOREP_Thread_LocationData*       location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    sourceRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent );


void
SCOREP_Tracing_MpiCollectiveBegin( SCOREP_Thread_LocationData* location,
                                   uint64_t                    timestamp );


void
SCOREP_Tracing_MpiCollectiveEnd( SCOREP_Thread_LocationData*       location,
                                 uint64_t                          timestamp,
                                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                                 SCOREP_MpiRank                    rootRank,
                                 SCOREP_MpiCollectiveType          collectiveType,
                                 uint64_t                          bytesSent,
                                 uint64_t                          bytesReceived );


void
SCOREP_Tracing_MpiIsendComplete( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 SCOREP_MpiRequestId         requestId );


void
SCOREP_Tracing_MpiIrecvRequest( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_MpiRequestId         requestId );


void
SCOREP_Tracing_MpiRequestTested( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 SCOREP_MpiRequestId         requestId );


void
SCOREP_Tracing_MpiRequestCancelled( SCOREP_Thread_LocationData* location,
                                    uint64_t                    timestamp,
                                    SCOREP_MpiRequestId         requestId );


void
SCOREP_Tracing_MpiIsend(  SCOREP_Thread_LocationData*       location,
                          uint64_t                          timestamp,
                          SCOREP_MpiRank                    destinationRank,
                          SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                          uint32_t                          tag,
                          uint64_t                          bytesSent,
                          SCOREP_MpiRequestId               requestId );


void
SCOREP_Tracing_MpiIrecv( SCOREP_Thread_LocationData*       location,
                         uint64_t                          timestamp,
                         SCOREP_MpiRank                    sourceRank,
                         SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                         uint32_t                          tag,
                         uint64_t                          bytesReceived,
                         SCOREP_MpiRequestId               requestId );


void
SCOREP_Tracing_OmpFork( SCOREP_Thread_LocationData* location,
                        uint64_t                    timestamp,
                        uint32_t                    nRequestedThreads );


void
SCOREP_Tracing_OmpJoin( SCOREP_Thread_LocationData* location,
                        uint64_t                    timestamp );


void
SCOREP_Tracing_OmpAcquireLock( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               uint32_t                    lockId,
                               uint32_t                    acquisitionOrder );


void
SCOREP_Tracing_OmpReleaseLock( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               uint32_t                    lockId,
                               uint32_t                    acquisitionOrder );


void
SCOREP_Tracing_OmpTaskCreate( SCOREP_Thread_LocationData* location,
                              uint64_t                    timestamp,
                              uint64_t                    taskId );

void
SCOREP_Tracing_OmpTaskSwitch( SCOREP_Thread_LocationData* location,
                              uint64_t                    timestamp,
                              uint64_t                    taskId );

void
SCOREP_Tracing_OmpTaskComplete( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                uint64_t                    taskId );


void
SCOREP_Tracing_ParameterInt64( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               SCOREP_ParameterHandle      parameterHandle,
                               int64_t                     value );

void
SCOREP_Tracing_ParameterUint64( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_ParameterHandle      parameterHandle,
                                uint64_t                    value );


void
SCOREP_Tracing_ParameterString( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_ParameterHandle      parameterHandle,
                                SCOREP_StringHandle         stringHandle );


#endif /* SCOREP_TRACING_EVENTS_H */
