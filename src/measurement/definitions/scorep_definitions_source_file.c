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
#include <UTILS_IO.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_SourceFileHandle
define_source_file( SCOREP_DefinitionManager* definition_manager,
                    SCOREP_StringHandle       fileNameHandle );


static bool
equal_source_file( const SCOREP_SourceFileDef* existingDefinition,
                   const SCOREP_SourceFileDef* newDefinition );


/**
 * Associate a file name with a process unique file handle.
 */
SCOREP_SourceFileHandle
SCOREP_Definitions_NewSourceFile( const char* fileName )
{
    UTILS_DEBUG_ENTRY( "%s", fileName );

    SCOREP_Definitions_Lock();

    SCOREP_SourceFileHandle new_handle = define_source_file(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            fileName ? fileName : "<unknown source file>",
            UTILS_IO_SimplifyPath ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_source_file( SCOREP_SourceFileDef*         definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_source_file(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ) );
}


SCOREP_SourceFileHandle
define_source_file( SCOREP_DefinitionManager* definition_manager,
                    SCOREP_StringHandle       fileNameHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_SourceFileDef*   new_definition = NULL;
    SCOREP_SourceFileHandle new_handle     = SCOREP_INVALID_SOURCE_FILE;

    SCOREP_DEFINITION_ALLOC( SourceFile );

    new_definition->name_handle = fileNameHandle;
    new_definition->hash_value  = SCOREP_GET_HASH_OF_LOCAL_HANDLE( new_definition->name_handle, String );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SourceFile, source_file );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_SOURCE_FILE ) );
    }

    return new_handle;
}


bool
equal_source_file( const SCOREP_SourceFileDef* existingDefinition,
                   const SCOREP_SourceFileDef* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle;
}


/**
 * Gets read-only access to the name of the source file.
 *
 * @param handle A handle to the source file definition.
 *
 * @return source file name.
 */
const char*
SCOREP_SourceFileHandle_GetName( SCOREP_SourceFileHandle handle )
{
    SCOREP_SourceFileDef* source_file = SCOREP_LOCAL_HANDLE_DEREF( handle, SourceFile );

    return SCOREP_LOCAL_HANDLE_DEREF( source_file->name_handle, String )->string_data;
}
