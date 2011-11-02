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


/**
 * @status      alpha
 * @file        SCOREP_Events.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */

#include <config.h>
#include <SCOREP_Events.h>
#include <stdlib.h>
#include <stdio.h>

#include <scorep_utility/SCOREP_Debug.h>
#include <SCOREP_Timing.h>
#include <scorep_utility/SCOREP_Omp.h>
#include <otf2/otf2.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>

#include "scorep_runtime_management.h"
#include "scorep_types.h"
#include "scorep_trace_types.h"
#include "scorep_thread.h"
#include "scorep_status.h"
#include "scorep_definition_structs.h"
#include "scorep_definitions.h"

#include "SCOREP_Metric_Management.h"

extern bool                     scorep_recording_enabled;
extern SCOREP_SamplingSetHandle scorep_current_sampling_set;
extern uint8_t                  scorep_number_of_metrics;
extern OTF2_TypeID*             scorep_current_metric_types;

/**
 * Process a region enter event in the measurement system.
 */
static void
scorep_enter_region( uint64_t            timestamp,
                     SCOREP_RegionHandle regionHandle,
                     uint64_t*           metricValues )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

        if ( metricValues )
        {
            OTF2_EvtWriter_Metric( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( scorep_current_sampling_set, SamplingSet ),
                                   scorep_number_of_metrics,
                                   scorep_current_metric_types,
                                   ( OTF2_MetricValue* )metricValues );
        }

        OTF2_EvtWriter_Enter( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Enter( location, regionHandle,
                              SCOREP_Region_GetType( regionHandle ),
                              timestamp, metricValues );
    }
}


void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle )
{
    uint64_t  timestamp     = SCOREP_GetClockTicks();
    uint64_t* metric_values = NULL;
    if ( scorep_number_of_metrics )
    {
        metric_values = SCOREP_Metric_read();
    }

    scorep_enter_region( timestamp, regionHandle, metric_values );
}


/**
 * Process a region exit event in the measurement system.
 */
static void
scorep_exit_region( uint64_t            timestamp,
                    SCOREP_RegionHandle regionHandle,
                    uint64_t*           metricValues )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

        if ( metricValues )
        {
            OTF2_EvtWriter_Metric( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( scorep_current_sampling_set, SamplingSet ),
                                   scorep_number_of_metrics,
                                   scorep_current_metric_types,
                                   ( OTF2_MetricValue* )metricValues );
        }

        OTF2_EvtWriter_Leave( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Exit( location,
                             regionHandle,
                             timestamp,
                             metricValues );
    }
}


void
SCOREP_ExitRegion( SCOREP_RegionHandle regionHandle )
{
    uint64_t  timestamp     = SCOREP_GetClockTicks();
    uint64_t* metric_values = NULL;
    if ( scorep_number_of_metrics )
    {
        metric_values = SCOREP_Metric_read();
    }

    scorep_exit_region( timestamp, regionHandle, metric_values );
}


/**
 * Process an mpi send event in the measurement system.
 */
void
SCOREP_MpiSend( SCOREP_MpiRank                    destinationRank,
                SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                uint32_t                          tag,
                uint64_t                          bytesSent )
{
    SCOREP_BUG_ON( destinationRank < 0, "Invalid rank passed to SCOREP_MpiSend" );

    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Dst:%d Comm:%s Tag:%u Bytes:%llu",
                         destinationRank,
                         scorep_comm_to_string( stringBuffer,
                                                sizeof( stringBuffer ),
                                                "%x", communicatorHandle ),
                         tag,
                         ( unsigned long long )bytesSent );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiSend( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                timestamp,
                                destinationRank,
                                SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                tag,
                                bytesSent );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Process an mpi recv event in the measurement system.
 */
void
SCOREP_MpiRecv( SCOREP_MpiRank                    sourceRank,
                SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                uint32_t                          tag,
                uint64_t                          bytesReceived )
{
    SCOREP_BUG_ON( sourceRank < 0, "Invalid rank passed to SCOREP_MpiRecv" );

    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Src:%u Comm:%s Tag:%u Bytes:%llu",
                         sourceRank,
                         scorep_comm_to_string( stringBuffer,
                                                sizeof( stringBuffer ),
                                                "%x", communicatorHandle ),
                         tag,
                         ( unsigned long long )bytesReceived );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiRecv( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                timestamp,
                                sourceRank,
                                SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                tag,
                                bytesReceived );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

static OTF2_Mpi_CollectiveType
scorep_collective_to_otf2( SCOREP_MpiCollectiveType scorep_type )
{
    switch ( scorep_type )
    {
#define CONVERT( name ) \
    case SCOREP_COLLECTIVE_MPI_ ## name: \
        return OTF2_MPI_ ## name

        CONVERT( BARRIER );
        CONVERT( BCAST );
        CONVERT( GATHER );
        CONVERT( GATHERV );
        CONVERT( SCATTER );
        CONVERT( SCATTERV );
        CONVERT( ALLGATHER );
        CONVERT( ALLGATHERV );
        CONVERT( ALLTOALL );
        CONVERT( ALLTOALLV );
        CONVERT( ALLTOALLW );
        CONVERT( ALLREDUCE );
        CONVERT( REDUCE );
        CONVERT( REDUCE_SCATTER );
        CONVERT( REDUCE_SCATTER_BLOCK );
        CONVERT( SCAN );
        CONVERT( EXSCAN );

        default:
            SCOREP_BUG( "Unknown collective type" );
            return 0;

#undef CONVERT
    }
}


/**
 * Process an mpi collective begin event in the measurement system.
 */
uint64_t
SCOREP_MpiCollectiveBegin( SCOREP_RegionHandle               regionHandle,
                           SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                           SCOREP_MpiRank                    rootRank,
                           SCOREP_MpiCollectiveType          collectiveType,
                           uint64_t                          matchingId )
{
    SCOREP_BUG_ON( ( rootRank < 0 && rootRank != SCOREP_INVALID_ROOT_RANK ),
                   "Invalid rank passed to SCOREP_MpiCollectiveBegin\n" );

    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();


    uint32_t root_rank;
    if ( rootRank == SCOREP_INVALID_ROOT_RANK )
    {
        root_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        root_rank = ( uint32_t )rootRank;
    }

    uint64_t* metric_values = NULL;
    if ( scorep_number_of_metrics )
    {
        metric_values = SCOREP_Metric_read();
    }

    SCOREP_DEBUG_ONLY( char stringBuffer[ 3 ][ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s Comm:%s Root:%s",
                         scorep_region_to_string( stringBuffer[ 0 ],
                                                  sizeof( stringBuffer[ 0 ] ),
                                                  "%x", regionHandle ),
                         scorep_comm_to_string( stringBuffer[ 1 ],
                                                sizeof( stringBuffer[ 1 ] ),
                                                "%x", communicatorHandle ),
                         scorep_uint32_to_string( stringBuffer[ 2 ],
                                                  sizeof( stringBuffer[ 2 ] ),
                                                  "%x", rootRank,
                                                  SCOREP_INVALID_ROOT_RANK ) );


    scorep_enter_region( timestamp, regionHandle, metric_values );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter* evt_writer
            = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

        OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                           NULL,
                                           timestamp,
                                           scorep_collective_to_otf2( collectiveType ),
                                           SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                           root_rank,
                                           matchingId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }

    return timestamp;
}

/**
 * Process an mpi collective end event in the measurement system.
 */
void
SCOREP_MpiCollectiveEnd( SCOREP_RegionHandle               regionHandle,
                         SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                         uint64_t                          matchingId,
                         uint64_t                          bytesSent,
                         uint64_t                          bytesReceived )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    uint64_t*                   metric_values = NULL;
    if ( scorep_number_of_metrics )
    {
        metric_values = SCOREP_Metric_read();
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter* evt_writer
            = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

        OTF2_EvtWriter_MpiCollectiveEnd( evt_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                         matchingId,
                                         bytesSent,
                                         bytesReceived );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }

    scorep_exit_region( timestamp, regionHandle, metric_values );
}

void
SCOREP_MpiIsendComplete( SCOREP_MpiRequestId requestId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiIsendComplete( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                         NULL,
                                         timestamp,
                                         requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

void
SCOREP_MpiIrecvRequest( SCOREP_MpiRequestId requestId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiIrecvRequest( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                        NULL,
                                        timestamp,
                                        requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

void
SCOREP_MpiRequestTested( SCOREP_MpiRequestId requestId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiRequestTest( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                       NULL,
                                       timestamp,
                                       requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

void
SCOREP_MpiRequestCancelled( SCOREP_MpiRequestId requestId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiRequestCancelled( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                            NULL,
                                            timestamp,
                                            requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

void
SCOREP_MpiIsend(  SCOREP_MpiRank                    destinationRank,
                  SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                  uint32_t                          tag,
                  uint64_t                          bytesSent,
                  SCOREP_MpiRequestId               requestId )
{
    SCOREP_BUG_ON( destinationRank < 0, "Invalid rank passed to SCOREP_MpiIsend\n" );

    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiIsend( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 timestamp,
                                 destinationRank,
                                 SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                 tag,
                                 bytesSent,
                                 requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

void
SCOREP_MpiIrecv( SCOREP_MpiRank                    sourceRank,
                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                 uint32_t                          tag,
                 uint64_t                          bytesReceived,
                 SCOREP_MpiRequestId               requestId )
{
    SCOREP_BUG_ON( sourceRank < 0,  "Invalid rank passed to SCOREP_MpiIrecv\n" );

    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_MpiIrecv( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 timestamp,
                                 sourceRank,
                                 SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                 tag,
                                 bytesReceived,
                                 requestId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

/**
 * Process an OpenMP fork event in the measurement system.
 */
void
SCOREP_OmpFork( SCOREP_RegionHandle regionHandle,
                uint32_t            nRequestedThreads )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    SCOREP_Thread_OnThreadFork( nRequestedThreads );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpFork( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                timestamp,
                                nRequestedThreads,
                                SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnFork( location, nRequestedThreads );
    }
}


/**
 * Process an OpenMP join event in the measurement system.
 */
void
SCOREP_OmpJoin( SCOREP_RegionHandle regionHandle )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    SCOREP_Thread_OnThreadJoin();

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpJoin( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                timestamp,
                                SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
        SCOREP_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }

    /*
     * The OpenMP implementation on JUMP uses
     * its own exit handler. This exit handler is registered after our
     * Score-P exit handler in the test cases. Causing segmentation faults
     * during finalization, due to invalid TPD variables. Thus, we register
     * an exit handler in the join event, which ensures that the Score-P
     * finalization can access threadprivate variables.
     */
    static bool is_exit_handler_registered = false;
    if ( !is_exit_handler_registered && !omp_in_parallel() )
    {
        is_exit_handler_registered = true;
        SCOREP_RegisterExitHandler();
    }
}


/**
 * Process an OpenMP acquire lock event in the measurement system.
 */
void
SCOREP_OmpAcquireLock( uint32_t lockId/*,
                                         uint32_t acquire_release_count*/)
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpALock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpALock( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 timestamp,
                                 lockId,
                                 dummy_acquire_release_count,
                                 dummy_region_id );
        SCOREP_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Process an OpenMP release lock event in the measurement system.
 */
void
SCOREP_OmpReleaseLock( uint32_t lockId/*,
                                         uint32_t acquire_release_count*/)
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpRLock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpRLock( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 timestamp,
                                 lockId,
                                 dummy_acquire_release_count,
                                 dummy_region_id );
        SCOREP_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}

/**
 * Process an OpenMP TaskCreateBegin event in the measurement system.
 */
void
SCOREP_OmpTaskCreateBegin( SCOREP_RegionHandle regionHandle,
                           uint64_t            taskId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpTaskCreateBegin( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                           NULL,
                                           timestamp,
                                           SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ),
                                           taskId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskCreateBegin( regionHandle, taskId, timestamp );
    }
}

/**
 * Process an OpenMP TaskCreateEnd event in the measurement system.
 */
void
SCOREP_OmpTaskCreateEnd( SCOREP_RegionHandle regionHandle,
                         uint64_t            taskId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpTaskCreateEnd( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ),
                                         taskId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskCreateEnd( regionHandle, taskId, timestamp );
    }
}

/**
 * Process an OpenMP TaskBegin event in the measurement system.
 */
void
SCOREP_OmpTaskBegin( SCOREP_RegionHandle regionHandle,
                     uint64_t            taskId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );
    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpTaskBeginOrResume( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ),
                                             taskId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskBegin( regionHandle, taskId, timestamp );
    }
}

/**
 * Process an OpenMP TaskResume event in the measurement system.
 */
void
SCOREP_OmpTaskResume( SCOREP_RegionHandle regionHandle,
                      uint64_t            taskId )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );
    // don't write resume events for implicit tasks (ID == 0)
    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled && ( taskId != 0 ) )
    {
        OTF2_EvtWriter_OmpTaskBeginOrResume( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ),
                                             taskId );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskResume( regionHandle, taskId, timestamp );
    }
}

/**
 * Process an OpenMP TaskSuspend event in the measurement system.
 */
void
SCOREP_OmpTaskSuspend( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        //nothing is done for Tracing, since the suspend event is unnessesary in the trace
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        // Do we need something for profiling? Profiling is not yet implemented anyway.
        // Thus, leave at for the moment, as it is.
    }
}

/**
 * Process an OpenMP TaskCompleted event in the measurement system.
 */
void
SCOREP_OmpTaskCompleted( SCOREP_RegionHandle regionHandle )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_OmpTaskCompleted( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskCompleted( regionHandle, timestamp );
    }
}


/**
 *
 */
void
SCOREP_ExitRegionOnException( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SCOREP_TriggerCounterInt64( SCOREP_SamplingSetHandle counterHandle,
                            int64_t                  value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSet_Definition* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    SCOREP_BUG_ON( sampling_set->number_of_metrics != 1,
                   "User sampling set with more than one metric" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_TypeID      value_type = OTF2_INT64_T;
        OTF2_MetricValue values;
        values.signed_int = value;

        OTF2_EvtWriter_Metric( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( counterHandle,
                                                          SamplingSet ),
                               1,
                               &value_type,
                               &values );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
    }
}


/**
 *
 */
void
SCOREP_TriggerCounterUint64( SCOREP_SamplingSetHandle counterHandle,
                             uint64_t                 value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSet_Definition* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    SCOREP_BUG_ON( sampling_set->number_of_metrics != 1,
                   "User sampling set with more than one metric" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_TypeID      value_type = OTF2_UINT64_T;
        OTF2_MetricValue values;
        values.unsigned_int = value;

        OTF2_EvtWriter_Metric( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( counterHandle,
                                                          SamplingSet ),
                               1,
                               &value_type,
                               &values );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
    }
}


/**
 *
 */
void
SCOREP_TriggerCounterDouble( SCOREP_SamplingSetHandle counterHandle,
                             double                   value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSet_Definition* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    SCOREP_BUG_ON( sampling_set->number_of_metrics != 1,
                   "User sampling set with more than one metric" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_TypeID      value_type = OTF2_DOUBLE;
        OTF2_MetricValue values;
        values.floating_point = value;

        OTF2_EvtWriter_Metric( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( counterHandle,
                                                          SamplingSet ),
                               1,
                               &value_type,
                               &values );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerDouble( location,
                                      sampling_set->metric_handles[ 0 ],
                                      value );
    }
}


/**
 *
 */
void
SCOREP_TriggerMarker( SCOREP_MarkerHandle markerHandle )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SCOREP_TriggerParameterInt64( SCOREP_ParameterHandle parameterHandle,
                              int64_t                value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_ParameterInt( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                     NULL,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                     value );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ParameterInteger( location,
                                         parameterHandle,
                                         value );
    }
}


/**
 *
 */
void
SCOREP_TriggerParameterUint64( SCOREP_ParameterHandle parameterHandle,
                               uint64_t               value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_ParameterUnsignedInt( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                             value );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        /* The SCOREP_Profile_ParameterInteger handles unsigned and signed integers */
        SCOREP_Profile_ParameterInteger( location,
                                         parameterHandle,
                                         value );
    }
}


/**
 *
 */
void
SCOREP_TriggerParameterString( SCOREP_ParameterHandle parameterHandle,
                               const char*            value )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_StringHandle string_handle = SCOREP_DefineString( value );

    if ( SCOREP_IsTracingEnabled() && scorep_recording_enabled )
    {
        OTF2_EvtWriter_ParameterString( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                        NULL,
                                        timestamp,
                                        SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                        SCOREP_LOCAL_HANDLE_TO_ID( string_handle, String ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ParameterString( location,
                                        parameterHandle,
                                        string_handle );
    }
}

/**
 * Returns the time stamp of the last triggered event
 *
 */
uint64_t
SCOREP_GetLastTimeStamp()
{
    return SCOREP_GetClockTicks();
}
