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
 * @file       silc_trace_thread_interaction.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include "silc_trace_thread_interaction.h"

#include <SILC_Memory.h>
#include "silc_runtime_management.h"
#include "silc_definitions.h"
#include "silc_thread.h"
#include "silc_status.h"
#include "silc_mpi.h"
#include <stdlib.h>


SILC_Trace_LocationData*
SILC_Trace_CreateLocationData()
{
    // already checked at call-site
//    if ( !SILC_IsTracingEnabled() )
//    {
//        return 0;
//    }
    SILC_Trace_LocationData* new_data = SILC_Memory_AllocForMisc(
        sizeof( SILC_Trace_LocationData ) );
    // initialize in SILC_Trace_OnLocationCreation
    new_data->otf_writer   = 0;
    new_data->otf_location = OTF2_UNDEFINED_UINT64;
    return new_data;
}


void
SILC_Trace_DeleteLocationData( SILC_Trace_LocationData* traceLocationData )
{
    if ( traceLocationData && traceLocationData->otf_writer )
    {
        traceLocationData->otf_writer = 0;
        // writer will be deleted by otf in call to OTF2_Archive_Delete()
    }
}


void
SILC_Trace_OnThreadCreation( SILC_Thread_LocationData* locationData,
                             SILC_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SILC_IsTracingEnabled() )
//     {
//
//     }
}


void
SILC_Trace_OnThreadActivation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SILC_IsTracingEnabled() )
//     {
//
//     }
}


void
SILC_Trace_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                SILC_Thread_LocationData* parentLocationData )
{
    return;

//     if ( !SILC_IsTracingEnabled() )
//     {
//
//     }
}


void
SILC_Trace_OnLocationCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
    if ( !SILC_IsTracingEnabled() )
    {
        return;
    }

    SILC_Trace_LocationData* trace_data = SILC_Thread_GetTraceLocationData( locationData );

    #pragma omp critical (trace_on_location_creation)
    {
        trace_data->otf_writer = OTF2_Archive_GetEvtWriter( silc_otf2_archive,
                                                            OTF2_UNDEFINED_UINT64,
                                                            SILC_OnTracePreFlush,
                                                            SILC_OnTraceAndDefinitionPostFlush );
    }

    if ( !trace_data->otf_writer )
    {
        SILC_ERROR( SILC_ERROR_ENOMEM, "Can't create event buffer" );
        _Exit( EXIT_FAILURE );
    }

    if ( !SILC_Mpi_IsInitialized() )
    {
        // Global location id unknown because rank not accessible.
        // Deferred processing will take place in SILC_InitMeasurementMPI()
    }
    else
    {
        SILC_SetOtf2WriterLocationId( locationData );
    }
}


void
SILC_SetOtf2WriterLocationId( SILC_Thread_LocationData* threadLocationData )
{
    // Does this function needs locking? I don't think so, it operates just on local data.
    if ( !SILC_IsTracingEnabled() )
    {
        int i = 42;
        return;
    }

    SILC_Trace_LocationData* trace_data = SILC_Thread_GetTraceLocationData( threadLocationData );
    assert( trace_data->otf_location == OTF2_UNDEFINED_UINT64 );
    trace_data->otf_location = SILC_CalculateGlobalLocationId( threadLocationData );

    SILC_Error_Code error = OTF2_EvtWriter_SetLocationID( trace_data->otf_writer,
                                                          trace_data->otf_location );
    if ( SILC_SUCCESS != error )
    {
        _Exit( EXIT_FAILURE );
    }
}
