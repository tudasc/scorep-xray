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
#include <scorep_location_management.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>

static SCOREP_AttributeHandle
define_attribute( SCOREP_DefinitionManager* definition_manager,
                  SCOREP_StringHandle       nameHandle,
                  SCOREP_StringHandle       descriptionHandle,
                  SCOREP_AttributeType      type );

static void
initialize_attribute( SCOREP_AttributeDef*      definition,
                      SCOREP_DefinitionManager* definition_manager,
                      SCOREP_StringHandle       nameHandle,
                      SCOREP_StringHandle       descriptionHandle,
                      SCOREP_AttributeType      type );

static bool
equal_attribute( const SCOREP_AttributeDef* existingDefinition,
                 const SCOREP_AttributeDef* newDefinition );


SCOREP_AttributeHandle
SCOREP_Definitions_NewAttribute( const char*          name,
                                 const char*          description,
                                 SCOREP_AttributeType type )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_AttributeHandle new_handle = define_attribute(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown attribute>", NULL ),
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            description ? description : "", NULL ),
        type );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

uint32_t
SCOREP_AttributeHandle_GetId( SCOREP_AttributeHandle handle )
{
    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Attribute );
}

SCOREP_AttributeType
SCOREP_AttributeHandle_GetType( SCOREP_AttributeHandle handle )
{
    return SCOREP_HANDLE_DEREF( handle, Attribute,
                                SCOREP_Memory_GetLocalDefinitionPageManager() )->type;
}

void
scorep_definitions_unify_attribute( SCOREP_AttributeDef*          definition,
                                    SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_attribute(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->description_handle,
            String,
            handlesPageManager ),
        definition->type );
}

SCOREP_AttributeHandle
define_attribute( SCOREP_DefinitionManager* definition_manager,
                  SCOREP_StringHandle       nameHandle,
                  SCOREP_StringHandle       descriptionHandle,
                  SCOREP_AttributeType      type )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_AttributeDef*   new_definition = NULL;
    SCOREP_AttributeHandle new_handle     = SCOREP_INVALID_ATTRIBUTE;

    SCOREP_DEFINITION_ALLOC( Attribute );
    initialize_attribute( new_definition,
                          definition_manager,
                          nameHandle,
                          descriptionHandle,
                          type );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Attribute, attribute );

    /* no NewDefinitionHandle yet, as attributes are not substrate unaware */

    return new_handle;
}

void
initialize_attribute( SCOREP_AttributeDef*      definition,
                      SCOREP_DefinitionManager* definition_manager,
                      SCOREP_StringHandle       nameHandle,
                      SCOREP_StringHandle       descriptionHandle,
                      SCOREP_AttributeType      type )
{
    definition->name_handle = nameHandle;
    HASH_ADD_HANDLE( definition, name_handle, String );

    definition->description_handle = descriptionHandle;
    HASH_ADD_HANDLE( definition, description_handle, String );

    definition->type = type;
    HASH_ADD_POD( definition, type );
}

bool
equal_attribute( const SCOREP_AttributeDef* existingDefinition,
                 const SCOREP_AttributeDef* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle &&
           existingDefinition->description_handle == newDefinition->description_handle &&
           existingDefinition->type == newDefinition->type;
}
