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
 * @file       src/measurement/definitions/scorep_definitions_string.c
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


SCOREP_StringHandle
SCOREP_Definitions_NewString( const char* str )
{
    UTILS_DEBUG_ENTRY( "%s", str );

    SCOREP_Definitions_Lock();

    SCOREP_StringHandle new_handle = scorep_definitions_new_string(
        &scorep_local_definition_manager,
        str );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyStringDefinitionToUnified( SCOREP_StringDef*             definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );
    definition->unified = scorep_definitions_new_string(
        scorep_unified_definition_manager,
        definition->string_data );
}


bool
equal_string( const SCOREP_StringDef* existingDefinition,
              const SCOREP_StringDef* newDefinition )
{
    return existingDefinition->string_length == newDefinition->string_length
           && 0 == memcmp( existingDefinition->string_data,
                           newDefinition->string_data,
                           existingDefinition->string_length );
}


SCOREP_StringHandle
scorep_definitions_new_string( SCOREP_DefinitionManager* definition_manager,
                               const char*               str )
{
    assert( definition_manager );

    SCOREP_StringDef*   new_definition = NULL;
    SCOREP_StringHandle new_handle     = SCOREP_INVALID_STRING;

    /* 1) Get storage for new definition */
    size_t string_length = strlen( str );
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( String,
                                            char,
                                            string_length + 1 );

    /* 2) populate definitions attributes */

    /* we know the length of the string already, therefore we can use the
     * faster memcpy
     */
    memcpy( new_definition->string_data, str, string_length + 1 );
    new_definition->string_length = string_length;
    new_definition->hash_value    = jenkins_hash( str, string_length, 0 );

    /*
     * 3) search in existing definitions and return found
     *    - discard new if an old one was found
     *    - if not, link new one into the hash chain and into definition list
     */
    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( String, string );

    return new_handle;
}


const char*
SCOREP_StringHandle_Get( SCOREP_StringHandle handle )
{
    SCOREP_StringDef* str = SCOREP_LOCAL_HANDLE_DEREF( handle, String );

    return str->string_data;
}
