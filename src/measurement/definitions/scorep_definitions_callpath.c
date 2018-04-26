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
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_CallpathHandle
define_callpath( SCOREP_DefinitionManager* definition_manager,
                 SCOREP_CallpathHandle     parentCallpath,
                 bool                      withParameter,
                 SCOREP_RegionHandle       regionHandle,
                 SCOREP_ParameterHandle    parameterHandle,
                 int64_t                   integerValue,
                 SCOREP_StringHandle       stringHandle );


static void
initialize_callpath( SCOREP_CallpathDef*       definition,
                     SCOREP_DefinitionManager* definition_manager,
                     SCOREP_CallpathHandle     parentCallpath,
                     bool                      withParameter,
                     SCOREP_RegionHandle       regionHandle,
                     SCOREP_ParameterHandle    parameterHandle,
                     int64_t                   integerValue,
                     SCOREP_StringHandle       stringHandle );


static bool
equal_callpath( const SCOREP_CallpathDef* existingDefinition,
                const SCOREP_CallpathDef* newDefinition );


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpath( SCOREP_CallpathHandle parentCallpath,
                                SCOREP_RegionHandle   region )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = define_callpath(
        &scorep_local_definition_manager,
        parentCallpath,
        false,
        region,
        SCOREP_INVALID_PARAMETER,
        0,
        SCOREP_INVALID_STRING );

    SCOREP_Definitions_Unlock();


    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpathParameterInteger( SCOREP_CallpathHandle  parentCallpath,
                                                SCOREP_ParameterHandle callpathParameter,
                                                int64_t                integerValue )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = define_callpath(
        &scorep_local_definition_manager,
        parentCallpath,
        true,
        SCOREP_INVALID_REGION,
        callpathParameter,
        integerValue,
        SCOREP_INVALID_STRING );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpathParameterString( SCOREP_CallpathHandle  parentCallpath,
                                               SCOREP_ParameterHandle callpathParameter,
                                               SCOREP_StringHandle    stringHandle )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = define_callpath(
        &scorep_local_definition_manager,
        parentCallpath,
        true,
        SCOREP_INVALID_REGION,
        callpathParameter,
        0,
        stringHandle );

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

    SCOREP_RegionHandle    unified_region_handle    = SCOREP_INVALID_REGION;
    SCOREP_ParameterHandle unified_parameter_handle = SCOREP_INVALID_PARAMETER;
    int64_t                integer_value            = 0;
    SCOREP_StringHandle    unified_string_handle    = SCOREP_INVALID_STRING;

    if ( !definition->with_parameter )
    {
        if ( definition->callpath_argument.region_handle != SCOREP_INVALID_REGION )
        {
            unified_region_handle = SCOREP_HANDLE_GET_UNIFIED(
                definition->callpath_argument.region_handle,
                Callpath,
                handlesPageManager );
            UTILS_BUG_ON( unified_region_handle == SCOREP_INVALID_REGION,
                          "Invalid unification order of callpath definition: region not yet unified" );
        }
    }
    else
    {
        if ( definition->callpath_argument.parameter_handle
             != SCOREP_INVALID_PARAMETER )
        {
            SCOREP_ParameterDef* parameter = SCOREP_HANDLE_DEREF(
                definition->callpath_argument.parameter_handle,
                Parameter,
                handlesPageManager );

            unified_parameter_handle = parameter->unified;
            UTILS_BUG_ON( unified_parameter_handle == SCOREP_INVALID_PARAMETER,
                          "Invalid unification order of callpath definition: parameter not yet unified" );

            if ( ( parameter->parameter_type == SCOREP_PARAMETER_INT64 ) ||
                 ( parameter->parameter_type == SCOREP_PARAMETER_UINT64 ) )
            {
                integer_value = definition->parameter_value.integer_value;
            }
            else if ( parameter->parameter_type == SCOREP_PARAMETER_STRING )
            {
                if ( definition->parameter_value.string_handle != SCOREP_INVALID_STRING )
                {
                    unified_string_handle = SCOREP_HANDLE_GET_UNIFIED(
                        definition->parameter_value.string_handle,
                        String,
                        handlesPageManager );
                    UTILS_BUG_ON( unified_string_handle == SCOREP_INVALID_STRING,
                                  "Invalid unification order of callpath definition: string parameter not yet unified" );
                }
            }
            else
            {
                UTILS_BUG( "Not a valid parameter type: %u",
                           parameter->parameter_type );
            }
        }
    }

    definition->unified = define_callpath(
        scorep_unified_definition_manager,
        unified_parent_callpath_handle,
        definition->with_parameter,
        unified_region_handle,
        unified_parameter_handle,
        integer_value,
        unified_string_handle );
}


SCOREP_CallpathHandle
define_callpath( SCOREP_DefinitionManager* definition_manager,
                 SCOREP_CallpathHandle     parentCallpathHandle,
                 bool                      withParameter,
                 SCOREP_RegionHandle       regionHandle,
                 SCOREP_ParameterHandle    parameterHandle,
                 int64_t                   integerValue,
                 SCOREP_StringHandle       stringHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_CallpathDef*   new_definition = NULL;
    SCOREP_CallpathHandle new_handle     = SCOREP_INVALID_CALLPATH;
    SCOREP_DEFINITION_ALLOC( Callpath );
    initialize_callpath( new_definition,
                         definition_manager,
                         parentCallpathHandle,
                         withParameter,
                         regionHandle,
                         parameterHandle,
                         integerValue,
                         stringHandle );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Callpath, callpath );

    /* no NewDefinitionHandle, callpath are only defined at unification by profiling */

    return new_handle;
}


void
initialize_callpath( SCOREP_CallpathDef*       definition,
                     SCOREP_DefinitionManager* definition_manager,
                     SCOREP_CallpathHandle     parentCallpathHandle,
                     bool                      withParameter,
                     SCOREP_RegionHandle       regionHandle,
                     SCOREP_ParameterHandle    parameterHandle,
                     int64_t                   integerValue,
                     SCOREP_StringHandle       stringHandle )
{
    definition->parent_callpath_handle = parentCallpathHandle;
    if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( definition, parent_callpath_handle, Callpath );
    }

    definition->with_parameter = withParameter;
    HASH_ADD_POD( definition, with_parameter );

    if ( !definition->with_parameter )
    {
        definition->callpath_argument.region_handle = regionHandle;
        if ( definition->callpath_argument.region_handle
             != SCOREP_INVALID_REGION )
        {
            HASH_ADD_HANDLE( definition, callpath_argument.region_handle, Region );
        }
    }
    else
    {
        definition->callpath_argument.parameter_handle = parameterHandle;
        if ( definition->callpath_argument.parameter_handle
             != SCOREP_INVALID_PARAMETER )
        {
            HASH_ADD_HANDLE( definition, callpath_argument.parameter_handle, Parameter );

            SCOREP_ParameterDef* parameter = SCOREP_HANDLE_DEREF(
                definition->callpath_argument.parameter_handle,
                Parameter,
                definition_manager->page_manager );

            if ( ( parameter->parameter_type == SCOREP_PARAMETER_INT64 ) ||
                 ( parameter->parameter_type == SCOREP_PARAMETER_UINT64 ) )
            {
                definition->parameter_value.integer_value = integerValue;
                HASH_ADD_POD( definition, parameter_value.integer_value );
            }
            else if ( parameter->parameter_type == SCOREP_PARAMETER_STRING )
            {
                definition->parameter_value.string_handle = stringHandle;
                if ( definition->parameter_value.string_handle
                     != SCOREP_INVALID_STRING )
                {
                    HASH_ADD_HANDLE( definition,
                                     parameter_value.string_handle,
                                     String );
                }
            }
            else
            {
                UTILS_BUG( "Not a valid parameter type: %u",
                           parameter->parameter_type );
            }
        }
    }
}


bool
equal_callpath( const SCOREP_CallpathDef* existingDefinition,
                const SCOREP_CallpathDef* newDefinition )
{
    if ( existingDefinition->parent_callpath_handle != newDefinition->parent_callpath_handle
         || existingDefinition->with_parameter != newDefinition->with_parameter )
    {
        return false;
    }

    if ( !existingDefinition->with_parameter )
    {
        return existingDefinition->callpath_argument.region_handle == newDefinition->callpath_argument.region_handle;
    }

    if ( existingDefinition->callpath_argument.parameter_handle
         != newDefinition->callpath_argument.parameter_handle )
    {
        return false;
    }

    /** @note we need to deref parameter_handle to know the type,
     *        but we don't have the associated page manager
     *        thus, we use memcmp for comparison
     *        luckily, the union members have equal size, currently
     */
    return 0 == memcmp( &existingDefinition->parameter_value,
                        &newDefinition->parameter_value,
                        sizeof( existingDefinition->parameter_value ) );
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
