/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2019,
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


static SCOREP_IoHandleHandle
define_io_handle( SCOREP_DefinitionManager*        definition_manager,
                  SCOREP_StringHandle              name,
                  SCOREP_IoFileHandle              file,
                  SCOREP_IoParadigmType            ioParadigmType,
                  SCOREP_IoHandleFlag              flags,
                  SCOREP_InterimCommunicatorHandle scope,
                  SCOREP_IoHandleHandle            parent,
                  uint32_t                         unifyKey,
                  SCOREP_IoAccessMode              accessMode,
                  SCOREP_IoStatusFlag              statusFlags,
                  size_t                           sizeOfPayload,
                  void**                           payloadOut,
                  bool                             is_completed );

static void
initialize_io_handle( SCOREP_IoHandleDef*              definition,
                      SCOREP_DefinitionManager*        definition_manager,
                      SCOREP_StringHandle              name,
                      SCOREP_IoFileHandle              file,
                      SCOREP_IoParadigmType            ioParadigmType,
                      SCOREP_IoHandleFlag              flags,
                      SCOREP_InterimCommunicatorHandle scope,
                      SCOREP_IoHandleHandle            parent,
                      uint32_t                         unifyKey,
                      SCOREP_IoAccessMode              accessMode,
                      SCOREP_IoStatusFlag              statusFlags,
                      bool                             is_completed  );

static bool
equal_io_handle( const SCOREP_IoHandleDef* existingDefinition,
                 const SCOREP_IoHandleDef* newDefinition );

static void
hash_io_handle( SCOREP_IoHandleDef* definition );

static inline size_t
io_handle_static_size( void )
{
    return SCOREP_Allocator_RoundupToAlignment( sizeof( SCOREP_IoHandleDef ) );
}

/**
 * Associate a name, file, I/O paradigm and additional information
 * with a process unique handle of an I/O file. Mark whether this
 * handle is automatically created by the system (e.g., stdout).
 */
SCOREP_IoHandleHandle
SCOREP_Definitions_NewIoHandle( const char*                      name,
                                SCOREP_IoFileHandle              file,
                                SCOREP_IoParadigmType            ioParadigmType,
                                SCOREP_IoHandleFlag              flags,
                                SCOREP_InterimCommunicatorHandle scope,
                                SCOREP_IoHandleHandle            parent,
                                uint32_t                         unifyKey,
                                bool                             is_completed,
                                size_t                           sizeOfPayload,
                                void**                           payloadOut,
                                ... )
{
    SCOREP_IoAccessMode access_mode  = SCOREP_IO_ACCESS_MODE_NONE;
    SCOREP_IoStatusFlag status_flags = SCOREP_IO_STATUS_FLAG_NONE;

    if ( flags & SCOREP_IO_HANDLE_FLAG_PRE_CREATED )
    {
        va_list vl;
        va_start( vl, payloadOut );
        access_mode  = va_arg( vl, SCOREP_IoAccessMode );
        status_flags = va_arg( vl, SCOREP_IoStatusFlag );
        va_end( vl );
    }

    SCOREP_Definitions_Lock();

    SCOREP_IoHandleHandle new_handle = define_io_handle(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL ),
        file,
        ioParadigmType,
        flags,
        scope,
        parent,
        unifyKey,
        access_mode,
        status_flags,
        sizeOfPayload,
        payloadOut,
        is_completed );

    SCOREP_Definitions_Unlock();

    if ( is_completed )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_IO_HANDLE ) );
    }

    return new_handle;
}


SCOREP_IoHandleHandle
define_io_handle( SCOREP_DefinitionManager*        definition_manager,
                  SCOREP_StringHandle              name,
                  SCOREP_IoFileHandle              file,
                  SCOREP_IoParadigmType            ioParadigmType,
                  SCOREP_IoHandleFlag              flags,
                  SCOREP_InterimCommunicatorHandle scope,
                  SCOREP_IoHandleHandle            parent,
                  uint32_t                         unifyKey,
                  SCOREP_IoAccessMode              accessMode,
                  SCOREP_IoStatusFlag              statusFlags,
                  size_t                           sizeOfPayload,
                  void**                           payloadOut,
                  bool                             is_completed )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_IoHandleDef*   new_definition = NULL;
    SCOREP_IoHandleHandle new_handle     = SCOREP_INVALID_IO_HANDLE;

    size_t payload_offset = io_handle_static_size();
    size_t total_size     = payload_offset + sizeOfPayload;

    SCOREP_DEFINITION_ALLOC_SIZE( IoHandle, total_size );

    void* payload = ( char* )new_definition + payload_offset;

    initialize_io_handle( new_definition,
                          definition_manager,
                          name,
                          file,
                          ioParadigmType,
                          flags,
                          scope,
                          parent,
                          unifyKey,
                          accessMode,
                          statusFlags,
                          is_completed );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( IoHandle, io_handle );

    if ( sizeOfPayload && payloadOut )
    {
        *payloadOut = payload;
    }

    return new_handle;
}

void
initialize_io_handle( SCOREP_IoHandleDef*              definition,
                      SCOREP_DefinitionManager*        definition_manager,
                      SCOREP_StringHandle              nameHandle,
                      SCOREP_IoFileHandle              fileHandle,
                      SCOREP_IoParadigmType            ioParadigmType,
                      SCOREP_IoHandleFlag              flags,
                      SCOREP_InterimCommunicatorHandle scopeHandle,
                      SCOREP_IoHandleHandle            parentHandle,
                      uint32_t                         unifyKey,
                      SCOREP_IoAccessMode              accessMode,
                      SCOREP_IoStatusFlag              statusFlags,
                      bool                             is_completed )
{
    definition->name_handle      = nameHandle;
    definition->file_handle      = fileHandle;
    definition->io_paradigm_type = ioParadigmType;
    definition->flags            = flags;
    definition->scope_handle     = scopeHandle;
    definition->parent_handle    = parentHandle;
    definition->unify_key        = unifyKey;
    definition->access_mode      = accessMode;
    definition->status_flags     = statusFlags;
    definition->is_completed     = is_completed;

    hash_io_handle( definition );
}

static bool
equal_io_handle( const SCOREP_IoHandleDef* existingDefinition,
                 const SCOREP_IoHandleDef* newDefinition )
{
    /* Only definitions with non-zero keys may get unified */
    if ( 0 == ( existingDefinition->unify_key | newDefinition->unify_key ) )
    {
        return false;
    }

    return existingDefinition->name_handle      == newDefinition->name_handle &&
           existingDefinition->file_handle      == newDefinition->file_handle &&
           existingDefinition->io_paradigm_type == newDefinition->io_paradigm_type &&
           existingDefinition->flags            == newDefinition->flags &&
           existingDefinition->scope_handle     == newDefinition->scope_handle &&
           existingDefinition->parent_handle    == newDefinition->parent_handle &&
           existingDefinition->access_mode      == newDefinition->access_mode &&
           existingDefinition->status_flags     == newDefinition->status_flags &&
           existingDefinition->unify_key        == newDefinition->unify_key &&
           existingDefinition->is_completed     == newDefinition->is_completed;
}

void
scorep_definitions_unify_io_handle( SCOREP_IoHandleDef*                  definition,
                                    struct SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    if ( !definition->is_completed )
    {
        definition->unified = SCOREP_INVALID_IO_HANDLE;
        return;
    }

    SCOREP_StringHandle unified_name_handle = SCOREP_INVALID_STRING;
    if ( definition->name_handle != SCOREP_INVALID_STRING )
    {
        unified_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_name_handle == SCOREP_INVALID_STRING,
                      "Invalid unification order of I/O handle definition: name not yet unified" );
    }

    SCOREP_StringHandle unified_file_handle = SCOREP_INVALID_IO_FILE;
    if ( definition->file_handle != SCOREP_INVALID_IO_FILE )
    {
        unified_file_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->file_handle,
            IoFile,
            handlesPageManager );
        UTILS_BUG_ON( unified_file_handle == SCOREP_INVALID_IO_FILE,
                      "Invalid unification order of I/O handle definition: file not yet unified" );
    }

    SCOREP_SystemTreeNodeHandle unified_scope_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;
    if ( definition->scope_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        unified_scope_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->scope_handle,
            Communicator,
            handlesPageManager );
        UTILS_BUG_ON( unified_scope_handle == SCOREP_INVALID_INTERIM_COMMUNICATOR,
                      "Invalid unification order of I/O handle definition: scope not yet unified" );
    }

    SCOREP_IoHandleHandle unified_parent_handle = SCOREP_INVALID_IO_HANDLE;
    if ( definition->parent_handle != SCOREP_INVALID_IO_HANDLE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            IoHandle,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_IO_HANDLE,
                      "Invalid unification order of I/O handle definition: parent not yet unified" );
    }

    definition->unified = define_io_handle(
        scorep_unified_definition_manager,
        unified_name_handle,
        unified_file_handle,
        definition->io_paradigm_type,
        definition->flags,
        unified_scope_handle,
        unified_parent_handle,
        definition->unify_key,
        definition->access_mode,
        definition->status_flags,
        0,
        NULL,
        definition->is_completed  );
}

void
scorep_definitions_rehash_io_handle( SCOREP_IoHandleDef* definition )
{
    definition->hash_value = 0;

    hash_io_handle( definition );
}

void
hash_io_handle( SCOREP_IoHandleDef* definition )
{
    if ( definition->name_handle != SCOREP_INVALID_STRING )
    {
        HASH_ADD_HANDLE( definition, name_handle, String );
    }
    if ( definition->file_handle != SCOREP_INVALID_IO_FILE )
    {
        HASH_ADD_POD( definition, file_handle );
    }
    HASH_ADD_POD( definition, io_paradigm_type );
    HASH_ADD_POD( definition, flags );
    if ( definition->scope_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        HASH_ADD_HANDLE( definition, scope_handle, Communicator );
    }
    if ( definition->parent_handle != SCOREP_INVALID_IO_HANDLE )
    {
        HASH_ADD_HANDLE( definition, parent_handle, IoHandle );
    }
    HASH_ADD_POD( definition, access_mode );
    HASH_ADD_POD( definition, status_flags );
    HASH_ADD_POD( definition, unify_key );
}

SCOREP_IoFileHandle
SCOREP_IoHandleHandle_GetIoFile( SCOREP_IoHandleHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle )->file_handle;
}

void
SCOREP_IoHandleHandle_SetIoFile( SCOREP_IoHandleHandle handle,
                                 SCOREP_IoFileHandle   file )
{
    SCOREP_IoHandleDef* def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );

    UTILS_BUG_ON( def->is_completed, "SetIoFile on already completed I/O handle!" );

    SCOREP_Definitions_Lock();

    def->file_handle  = file;
    def->is_completed = true;

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( handle, SCOREP_HANDLE_TYPE_IO_HANDLE ) );
}

void*
SCOREP_IoHandleHandle_GetPayload( SCOREP_IoHandleHandle handle )
{
    size_t              payload_offset = io_handle_static_size();
    SCOREP_IoHandleDef* def            = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );

    return ( char* )def + payload_offset;
}
