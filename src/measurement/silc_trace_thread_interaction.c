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
 * @status ALPHA
 *
 *
 */


#include "silc_trace_thread_interaction.h"

#include <SILC_Memory.h>
#include "silc_runtime_management.h"
#include "silc_thread.h"
#include "silc_mpi.h"
#include <stdlib.h>


SILC_Trace_LocationData*
SILC_Trace_CreateLocationData()
{
    if ( !silc_tracing_enabled )
    {
        return 0;
    }
    SILC_Trace_LocationData* new_data = SILC_Memory_AllocForMultithreadedMisc(
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
    if ( !silc_tracing_enabled )
    {
        return;
    }
}


void
SILC_Trace_OnThreadActivation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
    if ( !silc_tracing_enabled )
    {
        return;
    }
}


void
SILC_Trace_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                SILC_Thread_LocationData* parentLocationData )
{
    if ( !silc_tracing_enabled )
    {
        return;
    }
}


void
SILC_Trace_OnLocationCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
    if ( !silc_tracing_enabled )
    {
        return;
    }

    SILC_Trace_LocationData* trace_data = SILC_Thread_GetTraceLocationData( locationData );
    trace_data->otf_location = SILC_GetOTF2LocationId( locationData );
    trace_data->otf_writer   = OTF2_Archive_GetEvtWriter( silc_otf2_archive,
                                                          OTF2_UNDEFINED_UINT64,
                                                          silc_on_trace_pre_flush,
                                                          silc_on_trace_post_flush );
    if ( !trace_data->otf_writer )
    {
        SILC_ERROR( SILC_ERROR_ENOMEM, "Can't create event buffer" );
        _Exit( EXIT_FAILURE );
    }
}
