/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2019,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager* definition_manager,
                 uint64_t                  globalLocationId,
                 SCOREP_StringHandle       nameHandle,
                 SCOREP_LocationType       locationType,
                 uint64_t                  numberOfEvents,
                 uint64_t                  locationGroupId,
                 size_t                    sizeOfPayload,
                 void**                    payloadOut );

/**
 * Registers a new location into the definitions.
 *
 * @in internal
 */
SCOREP_LocationHandle
SCOREP_Definitions_NewLocation( SCOREP_LocationType type,
                                const char*         name,
                                size_t              sizeOfPayload,
                                void**              payload )
{
    SCOREP_Definitions_Lock();

    SCOREP_LocationHandle new_handle = define_location(
        &scorep_local_definition_manager,
        UINT64_MAX,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL ),
        type, 0, 0,
        sizeOfPayload, payload );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_location( SCOREP_LocationDef*           definition,
                                   SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_location(
        scorep_unified_definition_manager,
        definition->global_location_id,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->location_type,
        definition->number_of_events,
        definition->location_group_id,
        0, NULL );
}


static inline bool
equal_location( const SCOREP_LocationDef* existingDefinition,
                const SCOREP_LocationDef* newDefinition )
{
    return false;
}


SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager* definition_manager,
                 uint64_t                  globalLocationId,
                 SCOREP_StringHandle       nameHandle,
                 SCOREP_LocationType       locationType,
                 uint64_t                  numberOfEvents,
                 uint64_t                  locationGroupId,
                 size_t                    sizeOfPayload,
                 void**                    payloadOut )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_LocationDef*   new_definition = NULL;
    SCOREP_LocationHandle new_handle     = SCOREP_INVALID_LOCATION;

    if ( payloadOut )
    {
        *payloadOut = NULL;
    }

    size_t payload_offset = SCOREP_Allocator_RoundupToAlignment( sizeof( SCOREP_LocationDef ) );
    new_handle     = SCOREP_Memory_AllocForDefinitions( NULL, payload_offset + sizeOfPayload );
    new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Location );
    memset( new_definition, 0, payload_offset + sizeOfPayload );
    SCOREP_INIT_DEFINITION_HEADER( new_definition );

    /* locations wont be unfied, therefore no hash value needed */
    new_definition->global_location_id = globalLocationId;
    new_definition->name_handle        = nameHandle;
    new_definition->location_type      = locationType;
    new_definition->number_of_events   = numberOfEvents;
    new_definition->location_group_id  = locationGroupId;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Location, location );

    if ( payloadOut )
    {
        *payloadOut = ( char* )new_definition + payload_offset;
    }

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_LOCATION ) );
    }

    return new_handle;
}
