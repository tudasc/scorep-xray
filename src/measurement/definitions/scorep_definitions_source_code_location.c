/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
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
#include <UTILS_IO.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>

static SCOREP_SourceCodeLocationHandle
define_source_code_location( SCOREP_DefinitionManager* definition_manager,
                             SCOREP_StringHandle       fileHandle,
                             SCOREP_LineNo             lineNumber );

static void
initialize_source_code_location( SCOREP_SourceCodeLocationDef* definition,
                                 SCOREP_DefinitionManager*     definition_manager,
                                 SCOREP_StringHandle           fileHandle,
                                 SCOREP_LineNo                 lineNumber );

static bool
equal_source_code_location( const SCOREP_SourceCodeLocationDef* existingDefinition,
                            const SCOREP_SourceCodeLocationDef* newDefinition );


SCOREP_SourceCodeLocationHandle
SCOREP_Definitions_NewSourceCodeLocation( const char*   file,
                                          SCOREP_LineNo lineNumber )
{
    UTILS_DEBUG_ENTRY( "%s:%d", file, lineNumber );

    SCOREP_Definitions_Lock();

    SCOREP_SourceCodeLocationHandle new_handle = define_source_code_location(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            file ? file : "",
            UTILS_IO_SimplifyPath ),
        lineNumber );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

uint32_t
SCOREP_SourceCodeLocationHandle_GetId( SCOREP_SourceCodeLocationHandle handle )
{
    return SCOREP_LOCAL_HANDLE_TO_ID( handle, SourceCodeLocation );
}

void
scorep_definitions_unify_source_code_location( SCOREP_SourceCodeLocationDef* definition,
                                               SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_source_code_location(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->file_handle,
            String,
            handlesPageManager ),
        definition->line_number );
}

SCOREP_SourceCodeLocationHandle
define_source_code_location( SCOREP_DefinitionManager* definition_manager,
                             SCOREP_StringHandle       fileHandle,
                             SCOREP_LineNo             lineNumber )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_SourceCodeLocationDef*   new_definition = NULL;
    SCOREP_SourceCodeLocationHandle new_handle     = SCOREP_INVALID_SOURCE_CODE_LOCATION;

    SCOREP_DEFINITION_ALLOC( SourceCodeLocation );
    initialize_source_code_location( new_definition,
                                     definition_manager,
                                     fileHandle,
                                     lineNumber );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SourceCodeLocation, source_code_location );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_SOURCE_CODE_LOCATION ) );
    }
    return new_handle;
}

void
initialize_source_code_location( SCOREP_SourceCodeLocationDef* definition,
                                 SCOREP_DefinitionManager*     definition_manager,
                                 SCOREP_StringHandle           fileHandle,
                                 SCOREP_LineNo                 lineNumber )
{
    definition->file_handle = fileHandle;
    HASH_ADD_HANDLE( definition, file_handle, String );

    definition->line_number = lineNumber;
    HASH_ADD_POD( definition, line_number );
}

bool
equal_source_code_location( const SCOREP_SourceCodeLocationDef* existingDefinition,
                            const SCOREP_SourceCodeLocationDef* newDefinition )
{
    return existingDefinition->file_handle == newDefinition->file_handle &&
           existingDefinition->line_number == newDefinition->line_number;
}
