/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
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

static SCOREP_CallpathHandle
define_callpath( SCOREP_DefinitionManager*                    definition_manager,
                 SCOREP_CallpathHandle                        parentCallpath,
                 SCOREP_RegionHandle                          regionHandle,
                 uint32_t                                     numberOfParameters,
                 const scorep_definitions_callpath_parameter* parameters );


static void
initialize_callpath( SCOREP_CallpathDef*                          definition,
                     SCOREP_DefinitionManager*                    definition_manager,
                     SCOREP_CallpathHandle                        parentCallpath,
                     SCOREP_RegionHandle                          regionHandle,
                     uint32_t                                     numberOfParameters,
                     const scorep_definitions_callpath_parameter* parameters );


static bool
equal_callpath( const SCOREP_CallpathDef* existingDefinition,
                const SCOREP_CallpathDef* newDefinition );


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpath( SCOREP_CallpathHandle                        parentCallpath,
                                SCOREP_RegionHandle                          region,
                                uint32_t                                     numberOfParameters,
                                const scorep_definitions_callpath_parameter* parameters )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = define_callpath(
        &scorep_local_definition_manager,
        parentCallpath,
        region,
        numberOfParameters,
        parameters );

    SCOREP_Definitions_Unlock();


    return new_handle;
}


void
scorep_definitions_unify_callpath( SCOREP_CallpathDef*           definition,
                                   SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_CallpathHandle unified_parent_callpath_handle = SCOREP_INVALID_CALLPATH;
    if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        unified_parent_callpath_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_callpath_handle,
            Callpath,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_callpath_handle == SCOREP_INVALID_CALLPATH,
                      "Invalid unification order of callpath definition: parent not yet unified" );
    }

    SCOREP_RegionHandle unified_region_handle = SCOREP_INVALID_REGION;
    if ( definition->region_handle != SCOREP_INVALID_REGION )
    {
        unified_region_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->region_handle,
            Callpath,
            handlesPageManager );
        UTILS_BUG_ON( unified_region_handle == SCOREP_INVALID_REGION,
                      "Invalid unification order of callpath definition: region not yet unified" );
    }

    scorep_definitions_callpath_parameter parameters[ definition->number_of_parameters ];
    /* Init the upper-byte of the string_handle union member to avoid memcmp
       of uninitialized memory */
    memset( parameters, 0, definition->number_of_parameters * sizeof( parameters[ 0 ] ) );

    for ( uint32_t i = 0; i < definition->number_of_parameters; i++ )
    {
        SCOREP_ParameterDef* parameter =
            SCOREP_HANDLE_DEREF( definition->parameters[ i ].parameter_handle,
                                 Parameter, handlesPageManager );
        UTILS_BUG_ON( parameter->unified == SCOREP_INVALID_PARAMETER,
                      "Invalid unification order of callpath definition: parameter not yet unified" );
        parameters[ i ].parameter_handle = parameter->unified;
        if ( parameter->parameter_type == SCOREP_PARAMETER_STRING )
        {
            parameters[ i ].parameter_value.string_handle = SCOREP_HANDLE_GET_UNIFIED(
                definition->parameters[ i ].parameter_value.string_handle,
                String, handlesPageManager );
            UTILS_BUG_ON( parameters[ i ].parameter_value.string_handle == SCOREP_INVALID_STRING,
                          "Invalid unification order of callpath definition: string not yet unified" );
        }
        else
        {
            parameters[ i ].parameter_value.integer_value =
                definition->parameters[ i ].parameter_value.integer_value;
        }
    }

    definition->unified = define_callpath(
        scorep_unified_definition_manager,
        unified_parent_callpath_handle,
        unified_region_handle,
        definition->number_of_parameters,
        parameters );
}


SCOREP_CallpathHandle
define_callpath( SCOREP_DefinitionManager*                    definition_manager,
                 SCOREP_CallpathHandle                        parentCallpathHandle,
                 SCOREP_RegionHandle                          regionHandle,
                 uint32_t                                     numberOfParameters,
                 const scorep_definitions_callpath_parameter* parameters )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_CallpathDef*   new_definition = NULL;
    SCOREP_CallpathHandle new_handle     = SCOREP_INVALID_CALLPATH;
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( Callpath,
                                            scorep_definitions_callpath_parameter,
                                            numberOfParameters );
    initialize_callpath( new_definition,
                         definition_manager,
                         parentCallpathHandle,
                         regionHandle,
                         numberOfParameters,
                         parameters );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Callpath, callpath );

    /* no NewDefinitionHandle, callpath are only defined at unification by profiling */

    return new_handle;
}


void
initialize_callpath( SCOREP_CallpathDef*                          definition,
                     SCOREP_DefinitionManager*                    definition_manager,
                     SCOREP_CallpathHandle                        parentCallpathHandle,
                     SCOREP_RegionHandle                          regionHandle,
                     uint32_t                                     numberOfParameters,
                     const scorep_definitions_callpath_parameter* parameters )
{
    definition->parent_callpath_handle = parentCallpathHandle;
    if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( definition, parent_callpath_handle, Callpath );
    }

    definition->region_handle = regionHandle;
    if ( definition->region_handle != SCOREP_INVALID_REGION )
    {
        HASH_ADD_HANDLE( definition, region_handle, Region );
    }

    definition->number_of_parameters = numberOfParameters;
    HASH_ADD_POD( definition, number_of_parameters );
    if ( definition->number_of_parameters == 0 )
    {
        return;
    }

    memcpy( definition->parameters, parameters, numberOfParameters * sizeof( *parameters ) );

    for ( uint32_t i = 0; i < numberOfParameters; i++ )
    {
        HASH_ADD_HANDLE( definition, parameters[ i ].parameter_handle, Parameter );
        if ( SCOREP_ParameterHandle_GetType( parameters[ i ].parameter_handle ) == SCOREP_PARAMETER_STRING )
        {
            HASH_ADD_HANDLE( definition, parameters[ i ].parameter_value.string_handle, String );
        }
        else
        {
            HASH_ADD_POD( definition, parameters[ i ].parameter_value.integer_value );
        }
    }
}


bool
equal_callpath( const SCOREP_CallpathDef* existingDefinition,
                const SCOREP_CallpathDef* newDefinition )
{
    return existingDefinition->parent_callpath_handle == newDefinition->parent_callpath_handle
           && existingDefinition->region_handle == newDefinition->region_handle
           && existingDefinition->number_of_parameters == newDefinition->number_of_parameters
           && 0 == memcmp( existingDefinition->parameters,
                           newDefinition->parameters,
                           existingDefinition->number_of_parameters * sizeof( *existingDefinition->parameters ) );
}


/**
 * Returns the sequence number of the unified definitions for a local callpath handle from
 * the mappings.
 * @param handle handle to local callpath handle.
 */
uint32_t
SCOREP_CallpathHandle_GetUnifiedId( SCOREP_CallpathHandle handle )
{
    uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( handle, Callpath );
    return scorep_local_definition_manager.callpath.mapping[ local_id ];
}


/**
 * Returns the unified handle from a local handle.
 */
SCOREP_CallpathHandle
SCOREP_CallpathHandle_GetUnified( SCOREP_CallpathHandle handle )
{
    return SCOREP_HANDLE_GET_UNIFIED( handle, Callpath,
                                      SCOREP_Memory_GetLocalDefinitionPageManager() );
}
