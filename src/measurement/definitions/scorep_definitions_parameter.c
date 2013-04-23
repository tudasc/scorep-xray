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
 * @file       src/measurement/definitions/scorep_definitions_parameter.c
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


static SCOREP_ParameterHandle
define_parameter( SCOREP_DefinitionManager* definition_manager,
                  SCOREP_StringHandle       nameHandle,
                  SCOREP_ParameterType      type );


static bool
equal_parameter( const SCOREP_Parameter_Definition* existingDefinition,
                 const SCOREP_Parameter_Definition* newDefinition );


/**
 * Associate a name and a type with a process unique parameter handle.
 */
SCOREP_ParameterHandle
SCOREP_DefineParameter( const char*          name,
                        SCOREP_ParameterType type )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_ParameterHandle new_handle = define_parameter(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "<unknown parameter>" ),
        type );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyParameterDefinitionToUnified( SCOREP_Parameter_Definition*  definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

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
    assert( definition_manager );

    SCOREP_Parameter_Definition* new_definition = NULL;
    SCOREP_ParameterHandle       new_handle     = SCOREP_INVALID_PARAMETER;

    SCOREP_DEFINITION_ALLOC( Parameter );
    new_definition->name_handle = nameHandle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );
    new_definition->parameter_type = type;
    HASH_ADD_POD( new_definition, parameter_type );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Parameter, parameter );

    return new_handle;
}


bool
equal_parameter( const SCOREP_Parameter_Definition* existingDefinition,
                 const SCOREP_Parameter_Definition* newDefinition )
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
SCOREP_Parameter_GetName( SCOREP_ParameterHandle handle )
{
    SCOREP_Parameter_Definition* param = SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter );

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
SCOREP_Parameter_GetType( SCOREP_ParameterHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter )->parameter_type;
}