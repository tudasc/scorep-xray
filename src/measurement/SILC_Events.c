/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @file        SILC_Events.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */

#include <config.h>
#include <SILC_Events.h>
#include <stdlib.h>
#include <stdio.h>

#include <silc_utility/SILC_Debug.h>
#include <SILC_Timing.h>
#include <silc_utility/SILC_Omp.h>
#include <otf2/OTF2_EvtWriter.h>
#include <SILC_Profile.h>
#include <SILC_Definitions.h>

#include "silc_runtime_management.h"
#include "silc_types.h"
#include "silc_trace_types.h"
#include "silc_thread.h"
#include "silc_definition_handles.h"
#include "silc_status.h"
#include "silc_parameter_registration.h"
#include "silc_definition_structs.h"



/**
 * Generate a region enter event in the measurement system.
 */
void
SILC_EnterRegion( SILC_RegionHandle regionHandle )
{
    uint64_t                  timestamp = SILC_GetClockTicks();
    SILC_Thread_LocationData* location  = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s",
                       silc_region_to_string( stringBuffer,
                                              sizeof( stringBuffer ),
                                              "%x", regionHandle ) );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_Enter( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                              NULL,
                              timestamp,
                              SILC_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_Enter( location, regionHandle,
                            SILC_Region_GetType( regionHandle ),
                            timestamp, NULL );
    }
}


/**
 * Generate a region exit event in the measurement system.
 */
void
SILC_ExitRegion( SILC_RegionHandle regionHandle )
{
    uint64_t                  timestamp = SILC_GetClockTicks();
    SILC_Thread_LocationData* location  = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s",
                       silc_region_to_string( stringBuffer,
                                              sizeof( stringBuffer ),
                                              "%x", regionHandle ) );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_Leave( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                              NULL,
                              timestamp,
                              SILC_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_Exit( location,
                           regionHandle,
                           timestamp,
                           NULL );
    }
}


/**
 * Generate an mpi send event in the measurement system.
 */
void
SILC_MpiSend( uint32_t                   globalDestinationRank,
              SILC_MPICommunicatorHandle communicatorHandle,
              uint32_t                   tag,
              uint64_t                   bytesSent )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Dst:%u Comm:%s Tag:%u Bytes:%llu",
                       globalDestinationRank,
                       silc_comm_to_string( stringBuffer,
                                            sizeof( stringBuffer ),
                                            "%x", communicatorHandle ),
                       tag,
                       ( unsigned long long )bytesSent );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiSend( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SILC_GetClockTicks(),
                                OTF2_MPI_BLOCK,
                                globalDestinationRank,
                                SILC_HANDLE_TO_ID( communicatorHandle, Group ),
                                tag,
                                bytesSent );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an mpi recv event in the measurement system.
 */
void
SILC_MpiRecv( uint32_t                   globalSourceRank,
              SILC_MPICommunicatorHandle communicatorHandle,
              uint32_t                   tag,
              uint64_t                   bytesReceived )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Src:%u Comm:%s Tag:%u Bytes:%llu",
                       globalSourceRank,
                       silc_comm_to_string( stringBuffer,
                                            sizeof( stringBuffer ),
                                            "%x", communicatorHandle ),
                       tag,
                       ( unsigned long long )bytesReceived );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiRecv( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SILC_GetClockTicks(),
                                OTF2_MPI_BLOCK,
                                globalSourceRank,
                                SILC_HANDLE_TO_ID( communicatorHandle, Group ),
                                tag,
                                bytesReceived );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an mpi collective event in the measurement system.
 */
void
SILC_MpiCollective( SILC_RegionHandle          regionHandle,
                    SILC_MPICommunicatorHandle communicatorHandle,
                    uint32_t                   globalRootRank,
                    uint64_t                   bytesSent,
                    uint64_t                   bytesReceived )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 3 ][ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s Comm:%s Root:%s Sent:%llu Recv:%llu",
                       silc_region_to_string( stringBuffer[ 0 ],
                                              sizeof( stringBuffer[ 0 ] ),
                                              "%x", regionHandle ),
                       silc_comm_to_string( stringBuffer[ 1 ],
                                            sizeof( stringBuffer[ 1 ] ),
                                            "%x", communicatorHandle ),
                       silc_uint32_to_string( stringBuffer[ 2 ],
                                              sizeof( stringBuffer[ 2 ] ),
                                              "%x", globalRootRank,
                                              SILC_INVALID_ROOT_RANK ),
                       ( unsigned long long )bytesSent,
                       ( unsigned long long )bytesReceived );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_MpiCollective( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                      NULL,
                                      SILC_GetClockTicks(),
                                      OTF2_MPI_BARRIER,
                                      SILC_HANDLE_TO_ID( communicatorHandle, Group ),
                                      globalRootRank,
                                      bytesSent,
                                      bytesReceived );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an OpenMP fork event in the measurement system.
 */
void
SILC_OmpFork( SILC_RegionHandle regionHandle,
              uint32_t          nRequestedThreads )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s",
                       silc_region_to_string( stringBuffer,
                                              sizeof( stringBuffer ),
                                              "%x", regionHandle ) );

    SILC_Thread_OnThreadFork( nRequestedThreads );

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_OmpFork( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SILC_GetClockTicks(),
                                nRequestedThreads,
                                SILC_HANDLE_TO_ID( regionHandle, Region ) );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_OnFork( location, nRequestedThreads );
    }
}


/**
 * Generate an OpenMP join event in the measurement system.
 */
void
SILC_OmpJoin( SILC_RegionHandle regionHandle )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s",
                       silc_region_to_string( stringBuffer,
                                              sizeof( stringBuffer ),
                                              "%x", regionHandle ) );

    SILC_Thread_OnThreadJoin();

    if ( SILC_IsTracingEnabled() )
    {
        OTF2_EvtWriter_OmpJoin( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                NULL,
                                SILC_GetClockTicks(),
                                SILC_HANDLE_TO_ID( regionHandle, Region ) );
        /// @todo better write n join events and pass the locationids of the joined
        /// threads (the master/father is implicitly given). let the master-thread-join
        /// be the last one to indicate the the corresponding fork can be closed.
        SILC_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an OpenMP acquire lock event in the measurement system.
 */
void
SILC_OmpAcquireLock( uint32_t lockId/*,
                                       uint32_t acquire_release_count*/)
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SILC_IsTracingEnabled() )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpALock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpALock( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 SILC_GetClockTicks(),
                                 lockId,
                                 dummy_acquire_release_count,
                                 dummy_region_id );
        SILC_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 * Generate an OpenMP release lock event in the measurement system.
 */
void
SILC_OmpReleaseLock( uint32_t lockId/*,
                                       uint32_t acquire_release_count*/)
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( SILC_IsTracingEnabled() )
    {
        uint32_t dummy_acquire_release_count = 0;
        uint32_t dummy_region_id             = 0; // OTF2_EvtWriter_OmpRLock needs to be changed, there
                                                  // is no need for a region id here
        OTF2_EvtWriter_OmpRLock( SILC_Thread_GetTraceLocationData( location )->otf_writer,
                                 NULL,
                                 SILC_GetClockTicks(),
                                 lockId,
                                 dummy_acquire_release_count,
                                 dummy_region_id );
        SILC_DEBUG_PRINTF( 0, "Only partially implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SILC_ExitRegionOnException( SILC_RegionHandle regionHandle )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "Reg:%s",
                       silc_region_to_string( stringBuffer,
                                              sizeof( stringBuffer ),
                                              "%x", regionHandle ) );

    SILC_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SILC_IsTracingEnabled() )
    {
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SILC_TriggerCounterInt64( SILC_CounterHandle counterHandle,
                          int64_t            value )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    if ( SILC_IsTracingEnabled() )
    {
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_TriggerInteger( location, counterHandle, value );
    }
}


/**
 *
 */
void
SILC_TriggerCounterDouble( SILC_CounterHandle counterHandle,
                           double             value )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    if ( SILC_IsTracingEnabled() )
    {
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_TriggerDouble( location, counterHandle, value );
    }
}


/**
 *
 */
void
SILC_TriggerMarker( SILC_MarkerHandle markerHandle )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    SILC_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SILC_IsTracingEnabled() )
    {
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SILC_TriggerParameterInt64( SILC_ParameterHandle parameterHandle,
                            int64_t              value )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    if ( SILC_IsTracingEnabled() )
    {
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_ParameterInteger( location,
                                       parameterHandle,
                                       value );
    }
}


/**
 *
 */
void
SILC_TriggerParameterDouble( SILC_ParameterHandle parameterHandle,
                             double               value )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    SILC_DEBUG_PRINTF( 0, "Not yet implemented." );

    if ( SILC_IsTracingEnabled() )
    {
    }

    if ( SILC_IsProfilingEnabled() )
    {
    }
}


/**
 *
 */
void
SILC_TriggerParameterString( SILC_ParameterHandle parameterHandle,
                             const char*          value )
{
    SILC_Thread_LocationData* location = SILC_Thread_GetLocationData();
    SILC_DEBUG_PRINTF( SILC_DEBUG_EVENTS, "" );

    if ( SILC_IsTracingEnabled() )
    {
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }

    if ( SILC_IsProfilingEnabled() )
    {
        SILC_StringHandle string_handle = silc_get_parameter_string_handle( value );
        SILC_Profile_ParameterString( location,
                                      parameterHandle,
                                      string_handle );
    }
}

/**
 * Returns the time stamp of the last triggered event
 *
 */
uint64_t
SILC_GetLastTimeStamp()
{
    /// @todo this does not match the above comment
    return SILC_GetClockTicks();
}
