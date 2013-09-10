/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file
 *
 *
 *
 */


#include <config.h>


#include <stdlib.h>


#include <otf2/otf2.h>


#include <SCOREP_Memory.h>
#include <scorep_runtime_management.h>
#include <definitions/SCOREP_Definitions.h>
#include <scorep_location.h>
#include <scorep_status.h>


#include "scorep_tracing_internal.h"
#include "SCOREP_Tracing.h"


SCOREP_TracingData*
SCOREP_Tracing_CreateLocationData( SCOREP_Location* locationData )
{
    SCOREP_TracingData* new_data
        = SCOREP_Location_AllocForMisc( locationData, sizeof( *new_data ) );

    new_data->otf_writer   = 0;
    new_data->rewind_stack = 0;

    return new_data;
}


void
SCOREP_Tracing_DeleteLocationData( SCOREP_TracingData* traceLocationData )
{
    if ( traceLocationData && traceLocationData->otf_writer )
    {
        traceLocationData->otf_writer = 0;
        // writer will be deleted by otf in call to OTF2_Archive_Close()
    }
}


void
SCOREP_Tracing_OnThreadCreation( SCOREP_Location* locationData,
                                 SCOREP_Location* parentLocationData )
{
    return;
}


void
SCOREP_Tracing_OnLocationActivation( SCOREP_Location* locationData,
                                     SCOREP_Location* parentLocationData )
{
    return;
}


void
SCOREP_Tracing_OnLocationDeactivation( SCOREP_Location* locationData,
                                       SCOREP_Location* parentLocationData )
{
    return;
}


void
SCOREP_Tracing_OnLocationCreation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData )
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    SCOREP_TracingData* tracing_data = SCOREP_Location_GetTracingData( locationData );

    SCOREP_Tracing_LockArchive();
    {
        /* SCOREP_Tracing_GetEventWriter() aborts on error */
        tracing_data->otf_writer = SCOREP_Tracing_GetEventWriter();
    }
    SCOREP_Tracing_UnlockArchive();

    if ( SCOREP_Status_IsMppInitialized() )
    {
        SCOREP_Tracing_AssignLocationId( locationData );
    }
}


void
SCOREP_Tracing_AssignLocationId( SCOREP_Location* threadLocationData )
{
    // Does this function needs locking? I don't think so, it operates just on local data.
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    SCOREP_TracingData* tracing_data = SCOREP_Location_GetTracingData( threadLocationData );
    uint64_t            location_id  = SCOREP_Location_GetGlobalId( threadLocationData );

    OTF2_ErrorCode error = OTF2_EvtWriter_SetLocationID( tracing_data->otf_writer,
                                                         location_id );
    if ( OTF2_SUCCESS != error )
    {
        _Exit( EXIT_FAILURE );
    }
}
