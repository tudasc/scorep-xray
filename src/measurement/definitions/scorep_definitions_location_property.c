/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2015, 2019,
 * Technische Universitaet Dresden, Germany
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

#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>

static SCOREP_LocationPropertyHandle
define_location_property( SCOREP_DefinitionManager* definition_manager,
                          SCOREP_LocationHandle     locationHandle,
                          SCOREP_StringHandle       nameHandle,
                          SCOREP_StringHandle       valueHandle );

/**
 * Registers a new location into the definitions.
 *
 * @in internal
 */
SCOREP_LocationPropertyHandle
SCOREP_Definitions_NewLocationProperty( SCOREP_LocationHandle locationHandle,
                                        const char*           name,
                                        const char*           value )
{
    SCOREP_Definitions_Lock();

    SCOREP_LocationPropertyHandle new_handle = define_location_property(
        &scorep_local_definition_manager,
        locationHandle,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL ),
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            value ? value : "", NULL ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
scorep_definitions_unify_location_property( SCOREP_LocationPropertyDef*   definition,
                                            SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_location_property(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->location_handle,
            Location,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->value_handle,
            String,
            handlesPageManager ) );
}

static inline bool
equal_location_property( const SCOREP_LocationPropertyDef* existingDefinition,
                         const SCOREP_LocationPropertyDef* newDefinition )
{
    return ( existingDefinition->location_handle == newDefinition->location_handle ) &&
           ( existingDefinition->name_handle == newDefinition->name_handle );
}

SCOREP_LocationPropertyHandle
define_location_property( SCOREP_DefinitionManager* definition_manager,
                          SCOREP_LocationHandle     locationHandle,
                          SCOREP_StringHandle       nameHandle,
                          SCOREP_StringHandle       valueHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_LocationPropertyDef*   new_definition = NULL;
    SCOREP_LocationPropertyHandle new_handle     = SCOREP_INVALID_LOCATION_PROPERTY;

    SCOREP_DEFINITION_ALLOC( LocationProperty );

    /* locations wont be unfied, therefore no hash value needed */
    new_definition->location_handle = locationHandle;
    new_definition->name_handle     = nameHandle;
    new_definition->value_handle    = valueHandle;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( LocationProperty, location_property );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_LOCATION_PROPERTY ) );
    }
    return new_handle;
}
