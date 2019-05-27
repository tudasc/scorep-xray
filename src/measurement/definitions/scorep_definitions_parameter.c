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


static SCOREP_ParameterHandle
define_parameter( SCOREP_DefinitionManager* definition_manager,
                  SCOREP_StringHandle       nameHandle,
                  SCOREP_ParameterType      type );


static bool
equal_parameter( const SCOREP_ParameterDef* existingDefinition,
                 const SCOREP_ParameterDef* newDefinition );


/**
 * Associate a name and a type with a process unique parameter handle.
 */
SCOREP_ParameterHandle
SCOREP_Definitions_NewParameter( const char*          name,
                                 SCOREP_ParameterType type )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_ParameterHandle new_handle = define_parameter(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown parameter>", NULL ),
        type );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_parameter( SCOREP_ParameterDef*          definition,
                                    SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_parameter(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->parameter_type );
}


SCOREP_ParameterHandle
define_parameter( SCOREP_DefinitionManager* definition_manager,
                  SCOREP_StringHandle       nameHandle,
                  SCOREP_ParameterType      type )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_ParameterDef*   new_definition = NULL;
    SCOREP_ParameterHandle new_handle     = SCOREP_INVALID_PARAMETER;

    SCOREP_DEFINITION_ALLOC( Parameter );
    new_definition->name_handle = nameHandle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );
    new_definition->parameter_type = type;
    HASH_ADD_POD( new_definition, parameter_type );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Parameter, parameter );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_PARAMETER ) );
    }

    return new_handle;
}


bool
equal_parameter( const SCOREP_ParameterDef* existingDefinition,
                 const SCOREP_ParameterDef* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->parameter_type == newDefinition->parameter_type;
}


/**
 * Gets read-only access to the name of the parameter.
 *
 * @param handle A handle to the parameter.
 *
 * @return parameter name.
 */
const char*
SCOREP_ParameterHandle_GetName( SCOREP_ParameterHandle handle )
{
    SCOREP_ParameterDef* param = SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter );

    return SCOREP_LOCAL_HANDLE_DEREF( param->name_handle, String )->string_data;
}


/**
 * Returns the type of the parameter.
 *
 * @param handle A handle to the parameter.
 *
 * @return parameter type.
 */
SCOREP_ParameterType
SCOREP_ParameterHandle_GetType( SCOREP_ParameterHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter )->parameter_type;
}
