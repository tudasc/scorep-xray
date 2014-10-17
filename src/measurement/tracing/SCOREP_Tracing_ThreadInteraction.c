/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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


/**
 * @file
 *
 *
 *
 */


#include <config.h>


#include <stdlib.h>
#include <inttypes.h>


#include <otf2/otf2.h>


#include <SCOREP_Memory.h>
#include <scorep_runtime_management.h>
#include <SCOREP_Definitions.h>
#include <scorep_location.h>
#include <scorep_status.h>


#include "scorep_tracing_internal.h"
#include "SCOREP_Tracing.h"


SCOREP_TracingData*
SCOREP_Tracing_CreateLocationData( SCOREP_Location* locationData )
{
    SCOREP_TracingData* new_data
        = SCOREP_Location_AllocForMisc( locationData, sizeof( *new_data ) );

    new_data->otf_writer         = 0;
    new_data->rewind_stack       = 0;
    new_data->rewind_free_list   = 0;
    new_data->otf_attribute_list = OTF2_AttributeList_New();
    UTILS_BUG_ON( NULL == new_data->otf_attribute_list,
                  "Couldn't create event attribute list." );

    return new_data;
}


void
SCOREP_Tracing_DeleteLocationData( SCOREP_TracingData* traceLocationData )
{
    if ( traceLocationData )
    {
        traceLocationData->otf_writer = 0;
        // writer will be deleted by otf in call to OTF2_Archive_Close()

        OTF2_AttributeList_Delete( traceLocationData->otf_attribute_list );
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

    if ( scorep_tracing_use_sion && 0 != SCOREP_Location_GetId( locationData ) )
    {
        UTILS_FATAL( "Writing more than one location in a process is currently not "
                     "supported by the SIONlib support of OTF2. Please disable SIONlib via SCOREP_TRACING_USE_SION." );
    }

    SCOREP_TracingData* tracing_data = SCOREP_Location_GetTracingData( locationData );

    /* SCOREP_Tracing_GetEventWriter() aborts on error */
    tracing_data->otf_writer = SCOREP_Tracing_GetEventWriter();

    /* Attach the location to the evetn writer, so that we can access
     * it in case of an buffer flush.
     */
    OTF2_EvtWriter_SetUserData( tracing_data->otf_writer, locationData );
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

    SCOREP_Tracing_LockArchive();

    OTF2_ErrorCode error = OTF2_EvtWriter_SetLocationID( tracing_data->otf_writer,
                                                         location_id );
    if ( OTF2_SUCCESS != error )
    {
        UTILS_FATAL( "Could not set location id %" PRIu64 "to OTF2 event writer: %s",
                     location_id, OTF2_Error_GetName( error ) );
    }

    SCOREP_Tracing_UnlockArchive();
}
