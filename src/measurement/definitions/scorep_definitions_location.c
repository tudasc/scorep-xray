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
 * @status     alpha
 * @file       src/measurement/definitions/scorep_definitions_location.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
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
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager* definition_manager,
                 uint64_t                  globalLocationId,
                 SCOREP_LocationHandle     parent,
                 SCOREP_StringHandle       nameHandle,
                 SCOREP_LocationType       locationType,
                 uint64_t                  numberOfEvents,
                 uint64_t                  locationGroupId );

/**
 * Registers a new location into the definitions.
 *
 * @in internal
 */
SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t              globalLocationId,
                       SCOREP_LocationType   type,
                       SCOREP_LocationHandle parent,
                       const char*           name )
{
    SCOREP_Definitions_Lock();

    SCOREP_LocationHandle new_handle = define_location(
        &scorep_local_definition_manager,
        globalLocationId,
        parent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "" ),
        type, 0, 0 );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyLocationDefinitionToUnified( SCOREP_Location_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

    SCOREP_LocationHandle unified_parent_location_handle = SCOREP_INVALID_LOCATION;
    if ( definition->parent != SCOREP_INVALID_LOCATION )
    {
        unified_parent_location_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent,
            Location,
            handlesPageManager );
        assert( unified_parent_location_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = define_location(
        scorep_unified_definition_manager,
        definition->global_location_id,
        unified_parent_location_handle,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->location_type,
        definition->number_of_events,
        definition->location_group_id );
}


static inline bool
equal_location( const SCOREP_Location_Definition* existingDefinition,
                const SCOREP_Location_Definition* newDefinition )
{
    return false;
}


SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager* definition_manager,
                 uint64_t                  globalLocationId,
                 SCOREP_LocationHandle     parent,
                 SCOREP_StringHandle       nameHandle,
                 SCOREP_LocationType       locationType,
                 uint64_t                  numberOfEvents,
                 uint64_t                  locationGroupId )
{
    assert( definition_manager );

    SCOREP_Location_Definition* new_definition = NULL;
    SCOREP_LocationHandle       new_handle     = SCOREP_INVALID_LOCATION;

    SCOREP_DEFINITION_ALLOC( Location );

    /* locations wont be unfied, therefore no hash value needed */
    new_definition->global_location_id = globalLocationId;
    new_definition->parent             = parent;
    new_definition->name_handle        = nameHandle;
    new_definition->location_type      = locationType;
    new_definition->number_of_events   = numberOfEvents;
    new_definition->location_group_id  = locationGroupId;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Location, location );

    return new_handle;
}