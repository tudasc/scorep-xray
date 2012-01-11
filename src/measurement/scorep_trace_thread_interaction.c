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
 * @file       scorep_trace_thread_interaction.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include "scorep_trace_thread_interaction.h"

#include <SCOREP_Memory.h>
#include "scorep_runtime_management.h"
#include "scorep_definitions.h"
#include "scorep_thread.h"
#include "scorep_status.h"
#include "scorep_mpi.h"
#include <stdlib.h>


SCOREP_Trace_LocationData*
SCOREP_Trace_CreateLocationData()
{
    // already checked at call-site
//    if ( !SCOREP_IsTracingEnabled() )
//    {
//        return 0;
//    }
    SCOREP_Trace_LocationData* new_data = SCOREP_Memory_AllocForMisc(
        sizeof( SCOREP_Trace_LocationData ) );
    // initialize in SCOREP_Trace_OnLocationCreation
    new_data->otf_writer = 0;
    return new_data;
}


void
SCOREP_Trace_DeleteLocationData( SCOREP_Trace_LocationData* traceLocationData )
{
    if ( traceLocationData && traceLocationData->otf_writer )
    {
        traceLocationData->otf_writer = 0;
        // writer will be deleted by otf in call to OTF2_Archive_Delete()
    }
}


void
SCOREP_Trace_OnThreadCreation( SCOREP_Thread_LocationData* locationData,
                               SCOREP_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SCOREP_IsTracingEnabled() )
//     {
//
//     }
}


void
SCOREP_Trace_OnThreadActivation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SCOREP_IsTracingEnabled() )
//     {
//
//     }
}


void
SCOREP_Trace_OnThreadDectivation( SCOREP_Thread_LocationData* locationData,
                                  SCOREP_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SCOREP_IsTracingEnabled() )
//     {
//
//     }
}


void
SCOREP_Trace_OnLocationCreation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    SCOREP_Trace_LocationData* trace_data = SCOREP_Thread_GetTraceLocationData( locationData );

    #pragma omp critical (trace_on_location_creation)
    {
        /* SCOREP_Trace_GetEventWriter aborts on failure */
        trace_data->otf_writer =
            SCOREP_Trace_GetEventWriter( OTF2_UNDEFINED_UINT64 );
    }

    if ( !SCOREP_Mpi_IsInitialized() )
    {
        // Global location id unknown because rank not accessible.
        // Deferred processing will take place in SCOREP_InitMeasurementMPI()
    }
    else
    {
        SCOREP_SetOtf2WriterLocationId( locationData );
    }
}


void
SCOREP_SetOtf2WriterLocationId( SCOREP_Thread_LocationData* threadLocationData )
{
    // Does this function needs locking? I don't think so, it operates just on local data.
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    SCOREP_Trace_LocationData* trace_data = SCOREP_Thread_GetTraceLocationData( threadLocationData );
    //assert( threadLocationData->location_id == UINT64_MAX );
    uint64_t                   location_id = SCOREP_Thread_GetGlobalLocationId( threadLocationData );

    SCOREP_Error_Code          error = OTF2_EvtWriter_SetLocationID( trace_data->otf_writer,
                                                                     location_id );
    if ( SCOREP_SUCCESS != error )
    {
        _Exit( EXIT_FAILURE );
    }
}

uint64_t
SCOREP_Trace_GetNumberOfEvents( SCOREP_Thread_LocationData* locationData )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return 0;
    }

    SCOREP_Trace_LocationData* trace_data =
        SCOREP_Thread_GetTraceLocationData( locationData );

    uint64_t number_of_events;
    OTF2_EvtWriter_GetNumberOfEvents( trace_data->otf_writer,
                                      &number_of_events );

    return number_of_events;
}
