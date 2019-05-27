/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017, 2019,
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


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>

static SCOREP_IoFileHandle
define_io_file( SCOREP_DefinitionManager*   definition_manager,
                SCOREP_StringHandle         fileNameHandle,
                SCOREP_SystemTreeNodeHandle scope );

static void
initialize_io_file( SCOREP_IoFileDef*           definition,
                    SCOREP_DefinitionManager*   definition_manager,
                    SCOREP_StringHandle         fileNameHandle,
                    SCOREP_SystemTreeNodeHandle scope );

static bool
equal_io_file( const SCOREP_IoFileDef* existingDefinition,
               const SCOREP_IoFileDef* newDefinition );

/**
 * Registers a new I/O file into the definitions.
 */
SCOREP_IoFileHandle
SCOREP_Definitions_NewIoFile( const char*                 fileName,
                              SCOREP_SystemTreeNodeHandle scope )
{
    UTILS_DEBUG_ENTRY( "%s", fileName );

    SCOREP_Definitions_Lock();

    SCOREP_IoFileHandle new_handle = define_io_file(
        &scorep_local_definition_manager,
        /* name */
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            fileName ? fileName : "<unknown file>", NULL ),
        scope );

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_IO_FILE ) );

    return new_handle;
}

const char*
SCOREP_Definitions_GetIoFileName( SCOREP_IoFileHandle handle )
{
    SCOREP_IoFileDef* io_file = SCOREP_LOCAL_HANDLE_DEREF( handle, IoFile );

    return SCOREP_StringHandle_Get( io_file->file_name_handle );
}

SCOREP_IoFileHandle
define_io_file( SCOREP_DefinitionManager*   definition_manager,
                SCOREP_StringHandle         nameHandle,
                SCOREP_SystemTreeNodeHandle scope )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_IoFileDef*   new_definition = NULL;
    SCOREP_IoFileHandle new_handle     = SCOREP_INVALID_IO_FILE;

    SCOREP_DEFINITION_ALLOC( IoFile );

    initialize_io_file( new_definition,
                        definition_manager,
                        nameHandle,
                        scope );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( IoFile, io_file );

    return new_handle;
}

void
initialize_io_file( SCOREP_IoFileDef*           definition,
                    SCOREP_DefinitionManager*   definition_manager,
                    SCOREP_StringHandle         fileNameHandle,
                    SCOREP_SystemTreeNodeHandle scope )
{
    definition->file_name_handle = fileNameHandle;
    HASH_ADD_HANDLE( definition, file_name_handle, String );

    definition->scope = scope;
    HASH_ADD_POD( definition, scope );

    definition->properties      = SCOREP_INVALID_IO_FILE_PROPERTY;
    definition->properties_tail = &definition->properties;
}

bool
equal_io_file( const SCOREP_IoFileDef* existingDefinition,
               const SCOREP_IoFileDef* newDefinition )
{
    return existingDefinition->file_name_handle == newDefinition->file_name_handle &&
           existingDefinition->scope            == newDefinition->scope;
}


void
scorep_definitions_unify_io_file( SCOREP_IoFileDef*             definition,
                                  SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_StringHandle unified_file_name_handle = SCOREP_INVALID_STRING;
    if ( definition->file_name_handle != SCOREP_INVALID_STRING )
    {
        unified_file_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->file_name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_file_name_handle == SCOREP_MOVABLE_NULL,
                      "Invalid unification order of I/O file definition: file name not yet unified" );
    }

    SCOREP_SystemTreeNodeHandle unified_scope_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->scope != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_scope_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->scope,
            SystemTreeNode,
            handlesPageManager );
        UTILS_BUG_ON( unified_scope_handle == SCOREP_INVALID_SYSTEM_TREE_NODE,
                      "Invalid unification order of I/O file definition: scope not yet unified" );
    }

    definition->unified = define_io_file(
        scorep_unified_definition_manager,
        unified_file_name_handle,
        unified_scope_handle );
}
