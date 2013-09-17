/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
#include <scorep_location.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_Mutex interim_communicator_definition_counter_lock;


void
scorep_definitions_create_interim_communicator_counter_lock( void )
{
    SCOREP_MutexCreate( &interim_communicator_definition_counter_lock );
}


void
scorep_definitions_destroy_interim_communicator_counter_lock( void )
{
    SCOREP_MutexDestroy( &interim_communicator_definition_counter_lock );
}


static uint32_t
get_new_interim_communicator_id( void )
{
    uint32_t new_id;
    SCOREP_MutexLock( interim_communicator_definition_counter_lock );
    new_id = scorep_local_definition_manager.interim_communicator.counter++;
    SCOREP_MutexUnlock( interim_communicator_definition_counter_lock );
    return new_id;
}


static SCOREP_InterimCommunicatorHandle
define_interim_communicator( SCOREP_DefinitionManager*        definition_manager,
                             SCOREP_InterimCommunicatorHandle parentComm,
                             SCOREP_ParadigmType              paradigmType,
                             size_t                           sizeOfPayload,
                             void**                           payload );


static size_t
interim_comm_static_size()
{
    return SCOREP_Allocator_RoundupToAlignment(
               sizeof( SCOREP_InterimCommunicatorDef ) );
}


SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicator( SCOREP_InterimCommunicatorHandle parentComm,
                                           SCOREP_ParadigmType              paradigmType,
                                           size_t                           sizeOfPayload,
                                           void**                           payload )
{
    SCOREP_InterimCommunicatorHandle new_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    new_handle = define_interim_communicator(
        &scorep_local_definition_manager,
        parentComm,
        paradigmType,
        sizeOfPayload,
        payload );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void*
SCOREP_InterimCommunicatorHandle_GetPayload( SCOREP_InterimCommunicatorHandle handle )
{
    return ( char* )SCOREP_LOCAL_HANDLE_DEREF( handle,
                                               InterimCommunicator ) + interim_comm_static_size();
}


void
SCOREP_InterimCommunicatorHandle_SetName( SCOREP_InterimCommunicatorHandle commHandle,
                                          const char*                      name )
{
    UTILS_BUG_ON( commHandle == SCOREP_INVALID_INTERIM_COMMUNICATOR,
                  "Invalid InterimCommunicator handle as argument" );

    SCOREP_Definitions_Lock();

    SCOREP_InterimCommunicatorDef* definition = SCOREP_LOCAL_HANDLE_DEREF(
        commHandle,
        InterimCommunicator );

    if ( definition->name_handle == SCOREP_INVALID_STRING )
    {
        definition->name_handle = scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "" );
    }

    SCOREP_Definitions_Unlock();
}


SCOREP_InterimCommunicatorHandle
SCOREP_InterimCommunicatorHandle_GetParent( SCOREP_InterimCommunicatorHandle commHandle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( commHandle, InterimCommunicator )->parent_handle;
}


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicatorInLocation(
    SCOREP_Location*                     location,
    SCOREP_InterimCommunicatorHandle     parentComm,
    SCOREP_ParadigmType                  paradigmType,
    scorep_definitions_init_payload_fn   init_payload_fn,
    scorep_definitions_equal_payloads_fn equal_payloads_fn,
    scorep_definitions_manager_entry*    manager_entry,
    size_t                               sizeOfPayload,
    void**                               payloadOut,
    ... )
{
    SCOREP_InterimCommunicatorDef*   new_definition = NULL;
    SCOREP_InterimCommunicatorHandle new_handle     = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    size_t payload_offset = interim_comm_static_size();
    size_t total_size     = payload_offset + sizeOfPayload;

    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Location_GetMemoryPageManager( location,
                                              SCOREP_MEMORY_TYPE_DEFINITIONS );
    new_handle = SCOREP_Allocator_AllocMovable( page_manager, total_size );
    if ( new_handle == SCOREP_MOVABLE_NULL )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    new_definition = SCOREP_Allocator_GetAddressFromMovableMemory(
        page_manager,
        new_handle );
    SCOREP_INIT_DEFINITION_HEADER( new_definition );

    // Init new_definition
    new_definition->name_handle = SCOREP_INVALID_STRING;

    new_definition->parent_handle = parentComm;
    if ( parentComm != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        SCOREP_InterimCommunicatorDef* parent_definition =
            SCOREP_Allocator_GetAddressFromMovableMemory(
                page_manager,
                parentComm );
        new_definition->hash_value = jenkins_hashword(
            &parent_definition->hash_value,
            1,
            new_definition->hash_value );
    }

    new_definition->paradigm_type = paradigmType;
    HASH_ADD_POD( new_definition, paradigm_type );

    void* payload = ( char* )new_definition + payload_offset;
    if ( payloadOut )
    {
        /* indicates that the definition already existsed */
        *payloadOut = NULL;
    }

    if ( init_payload_fn )
    {
        va_list va;
        va_start( va, payloadOut );
        new_definition->hash_value =
            init_payload_fn( payload, new_definition->hash_value, va );
        va_end( va );
    }

    if ( manager_entry->hash_table && equal_payloads_fn )
    {
        SCOREP_InterimCommunicatorHandle* hash_table_bucket = &manager_entry->hash_table[
            new_definition->hash_value & manager_entry->hash_table_mask ];
        SCOREP_InterimCommunicatorHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SCOREP_MOVABLE_NULL )
        {
            SCOREP_InterimCommunicatorDef* existing_definition =
                SCOREP_Allocator_GetAddressFromMovableMemory(
                    page_manager,
                    hash_list_iterator );
            void* existing_payload = ( char* )existing_definition + interim_comm_static_size();
            if ( existing_definition->hash_value       == new_definition->hash_value
                 && existing_definition->name_handle   == new_definition->name_handle
                 && existing_definition->parent_handle == new_definition->parent_handle
                 && existing_definition->paradigm_type  == new_definition->paradigm_type
                 && equal_payloads_fn( existing_payload, payload ) )
            {
                SCOREP_Allocator_RollbackAllocMovable(
                    page_manager,
                    new_handle );
                return hash_list_iterator;
            }
            hash_list_iterator = existing_definition->hash_next;
        }
        new_definition->hash_next = *hash_table_bucket;
        *hash_table_bucket        = new_handle;
    }
    *manager_entry->tail            = new_handle;
    manager_entry->tail             = &new_definition->next;
    new_definition->sequence_number = get_new_interim_communicator_id();

    if ( sizeOfPayload && payloadOut )
    {
        *payloadOut = payload;
    }

    return new_handle;
}


static SCOREP_InterimCommunicatorHandle
define_interim_communicator( SCOREP_DefinitionManager*        definition_manager,
                             SCOREP_InterimCommunicatorHandle parentComm,
                             SCOREP_ParadigmType              paradigmType,
                             size_t                           sizeOfPayload,
                             void**                           payload )
{
    SCOREP_InterimCommunicatorDef*   new_definition = NULL;
    SCOREP_InterimCommunicatorHandle new_handle     = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    size_t payload_offset = interim_comm_static_size();
    size_t total_size     = payload_offset + sizeOfPayload;
    SCOREP_DEFINITION_ALLOC_SIZE( InterimCommunicator, total_size );

    // Init new_definition
    new_definition->name_handle   = SCOREP_INVALID_STRING;
    new_definition->parent_handle = parentComm;
    new_definition->paradigm_type = paradigmType;

    UTILS_BUG_ON( definition_manager->interim_communicator.hash_table,
                  "interim communicator definitions shouldn't have a hash table" );

    *definition_manager->interim_communicator.tail =
        new_handle;
    definition_manager->interim_communicator.tail = &new_definition->next;
    new_definition->sequence_number               = get_new_interim_communicator_id();

    if ( sizeOfPayload && payload )
    {
        *payload = ( char* )new_definition + payload_offset;
    }

    return new_handle;
}


static bool
equal_communicator( const SCOREP_CommunicatorDef* existingDefinition,
                    const SCOREP_CommunicatorDef* newDefinition );


static SCOREP_CommunicatorHandle
define_communicator( SCOREP_DefinitionManager* definition_manager,
                     SCOREP_GroupHandle        group_handle,
                     SCOREP_StringHandle       name_handle,
                     SCOREP_CommunicatorHandle parent_handle );


SCOREP_CommunicatorHandle
SCOREP_Definitions_NewCommunicator( SCOREP_GroupHandle        group_handle,
                                    const char*               name,
                                    SCOREP_CommunicatorHandle parent_handle )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CommunicatorHandle new_handle = define_communicator(
        &scorep_local_definition_manager,
        group_handle,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unnamed communicator>" ),
        parent_handle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_CommunicatorHandle
SCOREP_Definitions_NewUnifiedCommunicator( SCOREP_GroupHandle        group_handle,
                                           const char*               name,
                                           SCOREP_CommunicatorHandle parent_handle )
{
    UTILS_DEBUG_ENTRY();

    UTILS_ASSERT( scorep_unified_definition_manager );

    return define_communicator(
               scorep_unified_definition_manager,
               group_handle,
               scorep_definitions_new_string(
                   scorep_unified_definition_manager,
                   name ? name : "<unnamed communicator>" ),
               parent_handle );
}


void
scorep_definitions_unify_communicator( SCOREP_CommunicatorDef*       definition,
                                       SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

    SCOREP_CommunicatorHandle unified_parent_handle = SCOREP_INVALID_COMMUNICATOR;
    if ( definition->parent_handle != SCOREP_INVALID_COMMUNICATOR )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            Communicator,
            handlesPageManager );
        assert( unified_parent_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = define_communicator(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->group_handle,
            Group,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        unified_parent_handle );
}


SCOREP_CommunicatorHandle
define_communicator( SCOREP_DefinitionManager* definition_manager,
                     SCOREP_GroupHandle        group_handle,
                     SCOREP_StringHandle       name_handle,
                     SCOREP_CommunicatorHandle parent_handle )
{
    SCOREP_CommunicatorDef*   new_definition = NULL;
    SCOREP_CommunicatorHandle new_handle     = SCOREP_INVALID_COMMUNICATOR;

    SCOREP_DEFINITION_ALLOC( Communicator );

    // Init new_definition
    new_definition->group_handle  = group_handle;
    new_definition->name_handle   = name_handle;
    new_definition->parent_handle = parent_handle;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Communicator,
                                               communicator );

    return new_handle;
}


bool
equal_communicator( const SCOREP_CommunicatorDef* existingDefinition,
                    const SCOREP_CommunicatorDef* newDefinition )
{
    return false;
}
