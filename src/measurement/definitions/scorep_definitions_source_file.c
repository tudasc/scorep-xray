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
 * @file       src/measurement/definitions/scorep_source_file_definition.c
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


static SCOREP_SourceFileHandle
define_source_file( SCOREP_DefinitionManager* definition_manager,
                    SCOREP_StringHandle       fileNameHandle );


static bool
equal_source_file( const SCOREP_SourceFile_Definition* existingDefinition,
                   const SCOREP_SourceFile_Definition* newDefinition );


/**
 * Associate a file name with a process unique file handle.
 */
SCOREP_SourceFileHandle
SCOREP_DefineSourceFile( const char* fileName )
{
    UTILS_DEBUG_ENTRY( "%s", fileName );

    SCOREP_Definitions_Lock();

    SCOREP_SourceFileHandle new_handle = define_source_file(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            fileName ? fileName : "<unknown source file>" ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopySourceFileDefinitionToUnified( SCOREP_SourceFile_Definition* definition,
                                          SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

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
    assert( definition_manager );

    SCOREP_SourceFile_Definition* new_definition = NULL;
    SCOREP_SourceFileHandle       new_handle     = SCOREP_INVALID_SOURCE_FILE;

    SCOREP_DEFINITION_ALLOC( SourceFile );

    new_definition->name_handle = fileNameHandle;
    new_definition->hash_value  = SCOREP_GET_HASH_OF_LOCAL_HANDLE( new_definition->name_handle, String );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SourceFile, source_file );

    return new_handle;
}


bool
equal_source_file( const SCOREP_SourceFile_Definition* existingDefinition,
                   const SCOREP_SourceFile_Definition* newDefinition )
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
SCOREP_SourceFile_GetName( SCOREP_SourceFileHandle handle )
{
    SCOREP_SourceFile_Definition* source_file = SCOREP_LOCAL_HANDLE_DEREF( handle, SourceFile );

    return SCOREP_LOCAL_HANDLE_DEREF( source_file->name_handle, String )->string_data;
}
