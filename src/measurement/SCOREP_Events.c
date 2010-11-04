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
#include <otf2/OTF2_EvtWriter.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Definitions.h>

#include "scorep_runtime_management.h"
#include "scorep_types.h"
#include "scorep_trace_types.h"
#include "scorep_thread.h"
#include "scorep_definition_handles.h"
#include "scorep_status.h"
#include "scorep_parameter_registration.h"
#include "scorep_definition_structs.h"



/**
 * Generate a region enter event in the measurement system.
 */
void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_Enter( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Enter( location, regionHandle,
                              SCOREP_Region_GetType( regionHandle ),
                              timestamp, NULL );
    }
}


/**
 * Generate a region exit event in the measurement system.
 */
void
SCOREP_ExitRegion( SCOREP_RegionHandle regionHandle )
{
    uint64_t                    timestamp = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* location  = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_Leave( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Exit( location,
                             regionHandle,
                             timestamp,
                             NULL );
    }
}


/**
 * Generate an mpi send event in the measurement system.
 */
void
SCOREP_MpiSend( uint32_t                     globalDestinationRank,
                SCOREP_MPICommunicatorHandle communicatorHandle,
                uint32_t                     tag,
                uint64_t                     bytesSent )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Dst:%u Comm:%s Tag:%u Bytes:%llu",
                         globalDestinationRank,
                         scorep_comm_to_string( stringBuffer,
                                                sizeof( stringBuffer ),
                                                "%x", communicatorHandle ),
                         tag,
                         ( unsigned long long )bytesSent );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiSend( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SCOREP_GetClockTicks(),
                                OTF2_MPI_BLOCK,
                                globalDestinationRank,
                                SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, Group ),
                                tag,
                                bytesSent );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an mpi recv event in the measurement system.
 */
void
SCOREP_MpiRecv( uint32_t                     globalSourceRank,
                SCOREP_MPICommunicatorHandle communicatorHandle,
                uint32_t                     tag,
                uint64_t                     bytesReceived )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Src:%u Comm:%s Tag:%u Bytes:%llu",
                         globalSourceRank,
                         scorep_comm_to_string( stringBuffer,
                                                sizeof( stringBuffer ),
                                                "%x", communicatorHandle ),
                         tag,
                         ( unsigned long long )bytesReceived );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiRecv( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SCOREP_GetClockTicks(),
                                OTF2_MPI_BLOCK,
                                globalSourceRank,
                                SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, Group ),
                                tag,
                                bytesReceived );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an mpi collective event in the measurement system.
 */
void
SCOREP_MpiCollective( SCOREP_RegionHandle          regionHandle,
                      SCOREP_MPICommunicatorHandle communicatorHandle,
                      uint32_t                     globalRootRank,
                      uint64_t                     bytesSent,
                      uint64_t                     bytesReceived )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 3 ][ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s Comm:%s Root:%s Sent:%llu Recv:%llu",
                         scorep_region_to_string( stringBuffer[ 0 ],
                                                  sizeof( stringBuffer[ 0 ] ),
                                                  "%x", regionHandle ),
                         scorep_comm_to_string( stringBuffer[ 1 ],
                                                sizeof( stringBuffer[ 1 ] ),
                                                "%x", communicatorHandle ),
                         scorep_uint32_to_string( stringBuffer[ 2 ],
                                                  sizeof( stringBuffer[ 2 ] ),
                                                  "%x", globalRootRank,
                                                  SCOREP_INVALID_ROOT_RANK ),
                         ( unsigned long long )bytesSent,
                         ( unsigned long long )bytesReceived );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiCollective( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                      NULL,
                                      SCOREP_GetClockTicks(),
                                      OTF2_MPI_BARRIER,
                                      SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, Group ),
                                      globalRootRank,
                                      bytesSent,
                                      bytesReceived );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an OpenMP fork event in the measurement system.
 */
void
SCOREP_OmpFork( SCOREP_RegionHandle regionHandle,
                uint32_t            nRequestedThreads )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    SCOREP_Thread_OnThreadFork( nRequestedThreads );

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_OmpFork( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SCOREP_GetClockTicks(),
                                nRequestedThreads,
                                SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnFork( location, nRequestedThreads );
    }
}


/**
 * Generate an OpenMP join event in the measurement system.
 */
void
SCOREP_OmpJoin( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_ONLY( char stringBuffer[ 16 ];
                       )

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%s",
                         scorep_region_to_string( stringBuffer,
                                                  sizeof( stringBuffer ),
                                                  "%x", regionHandle ) );

    SCOREP_Thread_OnThreadJoin();

    if ( SCOREP_IsTracingEnabled() )
    {
        OTF2_EvtWriter_OmpJoin( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SCOREP_GetClockTicks(),
                                SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
        /// @todo better write n join events and pass the locationids of the joined
        /// threads (the master/father is implicitly given). let the master-thread-join
        /// be the last one to indicate the the corresponding fork can be closed.
        SCOREP_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an OpenMP acquire lock event in the measurement system.
 */
void
SCOREP_OmpAcquireLock( uint32_t lockId/*,
                                         uint32_t acquire_release_count*/)
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SCOREP_IsTracingEnabled() )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpALock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpALock( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 SCOREP_GetClockTicks(),
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
 * Generate an OpenMP release lock event in the measurement system.
 */
void
SCOREP_OmpReleaseLock( uint32_t lockId/*,
                                         uint32_t acquire_release_count*/)
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SCOREP_IsTracingEnabled() )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpRLock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpRLock( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 SCOREP_GetClockTicks(),
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

    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SCOREP_IsTracingEnabled() )
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
SCOREP_TriggerCounterInt64( SCOREP_CounterHandle counterHandle,
                            int64_t              value )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerInteger( location, counterHandle, value );
    }
}


/**
 *
 */
void
SCOREP_TriggerCounterDouble( SCOREP_CounterHandle counterHandle,
                             double               value )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerDouble( location, counterHandle, value );
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

    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SCOREP_IsTracingEnabled() )
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
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
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
SCOREP_TriggerParameterDouble( SCOREP_ParameterHandle parameterHandle,
                               double                 value )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SCOREP_IsTracingEnabled() )
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
SCOREP_TriggerParameterString( SCOREP_ParameterHandle parameterHandle,
                               const char*            value )
{
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_StringHandle string_handle = scorep_get_parameter_string_handle( value );
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
    /// @todo this does not match the above comment
    return SCOREP_GetClockTicks();
}
