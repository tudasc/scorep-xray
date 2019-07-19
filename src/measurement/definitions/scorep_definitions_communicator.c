/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2017-2018,
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
#include <scorep_location_management.h>
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
define_interim_communicator( SCOREP_Allocator_PageManager*        pageManager,
                             scorep_definitions_manager_entry*    managerEntry,
                             scorep_definitions_init_payload_fn   initPayloadFn,
                             scorep_definitions_equal_payloads_fn equalPayloadsFn,
                             size_t                               sizeOfPayload,
                             void**                               payloadOut,
                             SCOREP_InterimCommunicatorHandle     parentComm,
                             SCOREP_ParadigmType                  paradigmType,
                             va_list                              va );


static size_t
interim_comm_static_size( void )
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
    UTILS_DEBUG_ENTRY();

    return SCOREP_Definitions_NewInterimCommunicatorCustom(
        NULL,
        &scorep_local_definition_manager.interim_communicator,
        NULL,
        NULL,
        parentComm,
        paradigmType,
        sizeOfPayload,
        payload );
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
            name ? name : "", NULL );
    }

    SCOREP_Definitions_Unlock();
}


SCOREP_InterimCommunicatorHandle
SCOREP_InterimCommunicatorHandle_GetParent( SCOREP_InterimCommunicatorHandle commHandle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( commHandle, InterimCommunicator )->parent_handle;
}


SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicatorCustom(
    SCOREP_Location*                     location,
    scorep_definitions_manager_entry*    managerEntry,
    scorep_definitions_init_payload_fn   initPayloadFn,
    scorep_definitions_equal_payloads_fn equalPayloadsFn,
    SCOREP_InterimCommunicatorHandle     parentComm,
    SCOREP_ParadigmType                  paradigmType,
    size_t                               sizeOfPayload,
    void**                               payload,
    ... )
{
    va_list va;
    va_start( va, payload );

    SCOREP_Allocator_PageManager* page_manager;
    if ( !location )
    {
        /* No location given, we need to do the locking and using
           the common definition memory pool. */
        SCOREP_Definitions_Lock();
        page_manager = SCOREP_Memory_GetLocalDefinitionPageManager();
    }
    else
    {
        /*
         * Use the memory pool from the given location.
         * The caller needs to take care of the locking, if the
         * manager entry is shared.
         */
        page_manager = SCOREP_Location_GetOrCreateMemoryPageManager(
            location,
            SCOREP_MEMORY_TYPE_DEFINITIONS );
    }

    SCOREP_InterimCommunicatorHandle new_handle =
        define_interim_communicator(
            page_manager,
            managerEntry,
            initPayloadFn,
            equalPayloadsFn,
            sizeOfPayload,
            payload,
            parentComm,
            paradigmType,
            va );

    if ( !location )
    {
        SCOREP_Definitions_Unlock();
    }

    va_end( va );

    return new_handle;
}


SCOREP_InterimCommunicatorHandle
define_interim_communicator( SCOREP_Allocator_PageManager*        pageManager,
                             scorep_definitions_manager_entry*    managerEntry,
                             scorep_definitions_init_payload_fn   initPayloadFn,
                             scorep_definitions_equal_payloads_fn equalPayloadsFn,
                             size_t                               sizeOfPayload,
                             void**                               payloadOut,
                             SCOREP_InterimCommunicatorHandle     parentComm,
                             SCOREP_ParadigmType                  paradigmType,
                             va_list                              va )
{
    SCOREP_InterimCommunicatorDef*   new_definition = NULL;
    SCOREP_InterimCommunicatorHandle new_handle     = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    size_t payload_offset = interim_comm_static_size();
    size_t total_size     = payload_offset + sizeOfPayload;

    new_handle =
        SCOREP_Allocator_AllocMovable( pageManager, total_size );
    if ( new_handle == 0 )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    new_definition =
        SCOREP_Allocator_GetAddressFromMovableMemory( pageManager, new_handle );
    SCOREP_INIT_DEFINITION_HEADER( new_definition );

    // Init new_definition
    new_definition->name_handle = SCOREP_INVALID_STRING;

    new_definition->parent_handle = parentComm;
    if ( parentComm != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        SCOREP_InterimCommunicatorDef* parent_definition =
            SCOREP_Allocator_GetAddressFromMovableMemory(
                pageManager,
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
        /* indicates that the definition already existed */
        *payloadOut = NULL;
    }

    if ( initPayloadFn )
    {
        new_definition->hash_value =
            initPayloadFn( payload, new_definition->hash_value, va );
    }

    if ( equalPayloadsFn )
    {
        UTILS_BUG_ON( managerEntry->hash_table == NULL,
                      "No hash table allocated, even though a equal function was provided" );

        SCOREP_InterimCommunicatorHandle* hash_table_bucket = &managerEntry->hash_table[
            new_definition->hash_value & managerEntry->hash_table_mask ];
        SCOREP_InterimCommunicatorHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            SCOREP_InterimCommunicatorDef* existing_definition =
                SCOREP_Allocator_GetAddressFromMovableMemory(
                    pageManager,
                    hash_list_iterator );
            void* existing_payload = ( char* )existing_definition + interim_comm_static_size();
            if ( existing_definition->hash_value       == new_definition->hash_value
                 && existing_definition->name_handle   == new_definition->name_handle
                 && existing_definition->parent_handle == new_definition->parent_handle
                 && existing_definition->paradigm_type  == new_definition->paradigm_type
                 && equalPayloadsFn( existing_payload, payload ) )
            {
                SCOREP_Allocator_RollbackAllocMovable(
                    pageManager,
                    new_handle );
                return hash_list_iterator;
            }
            hash_list_iterator = existing_definition->hash_next;
        }
        new_definition->hash_next = *hash_table_bucket;
        *hash_table_bucket        = new_handle;
    }
    *managerEntry->tail             = new_handle;
    managerEntry->tail              = &new_definition->next;
    new_definition->sequence_number = get_new_interim_communicator_id();

    if ( sizeOfPayload && payloadOut )
    {
        *payloadOut = payload;
    }

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_INTERIM_COMMUNICATOR ) );

    return new_handle;
}

/* Caller needs to hold the definitions lock. */
uint32_t
scorep_definitions_interim_communicator_get_rma_window_creation_counter( SCOREP_InterimCommunicatorHandle communicatorHandle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( communicatorHandle, InterimCommunicator )->rma_window_creation_counter++;
}

static bool
equal_communicator( const SCOREP_CommunicatorDef* existingDefinition,
                    const SCOREP_CommunicatorDef* newDefinition );


static SCOREP_CommunicatorHandle
define_communicator( SCOREP_DefinitionManager* definition_manager,
                     SCOREP_GroupHandle        group_handle,
                     SCOREP_StringHandle       name_handle,
                     SCOREP_CommunicatorHandle parent_handle,
                     uint32_t                  unifyKey );


SCOREP_CommunicatorHandle
SCOREP_Definitions_NewCommunicator( SCOREP_GroupHandle        group,
                                    SCOREP_StringHandle       name,
                                    SCOREP_CommunicatorHandle parent,
                                    uint32_t                  unifyKey )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CommunicatorHandle new_handle = define_communicator(
        &scorep_local_definition_manager,
        group,
        name,
        parent,
        unifyKey );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_communicator( SCOREP_CommunicatorDef*       definition,
                                       SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_StringHandle unified_string_handle = SCOREP_INVALID_STRING;
    if ( definition->name_handle != SCOREP_INVALID_STRING )
    {
        unified_string_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_string_handle == SCOREP_INVALID_STRING,
                      "Invalid unification order of communicator definition: name not yet unified" );
    }

    SCOREP_CommunicatorHandle unified_parent_handle = SCOREP_INVALID_COMMUNICATOR;
    if ( definition->parent_handle != SCOREP_INVALID_COMMUNICATOR )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            Communicator,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_COMMUNICATOR,
                      "Invalid unification order of communicator definition: parent not yet unified" );
    }

    definition->unified = define_communicator(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->group_handle,
            Group,
            handlesPageManager ),
        unified_string_handle,
        unified_parent_handle,
        definition->unify_key );
}


SCOREP_CommunicatorHandle
define_communicator( SCOREP_DefinitionManager* definition_manager,
                     SCOREP_GroupHandle        group,
                     SCOREP_StringHandle       name,
                     SCOREP_CommunicatorHandle parent,
                     uint32_t                  unifyKey )
{
    SCOREP_CommunicatorDef*   new_definition = NULL;
    SCOREP_CommunicatorHandle new_handle     = SCOREP_INVALID_COMMUNICATOR;

    SCOREP_DEFINITION_ALLOC( Communicator );

    // Init new_definition
    new_definition->group_handle = group;
    HASH_ADD_HANDLE( new_definition, group_handle, Group );

    /* No hashing, wont be used to find duplicates */
    new_definition->name_handle = name;

    new_definition->parent_handle = parent;
    if ( new_definition->parent_handle != SCOREP_INVALID_COMMUNICATOR )
    {
        HASH_ADD_HANDLE( new_definition, parent_handle, Communicator );
    }

    new_definition->unify_key = unifyKey;
    HASH_ADD_POD( new_definition, unify_key );

    /* Open coded SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION, to handle the
     * communicator name */
    scorep_definitions_manager_entry* comms = &definition_manager->communicator;
    if ( comms->hash_table )
    {
        SCOREP_CommunicatorHandle* hash_table_bucket =
            &comms->hash_table[
                new_definition->hash_value & comms->hash_table_mask ];
        SCOREP_CommunicatorHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SCOREP_INVALID_COMMUNICATOR )
        {
            SCOREP_CommunicatorDef* existing_definition =
                SCOREP_Allocator_GetAddressFromMovableMemory(
                    definition_manager->page_manager,
                    hash_list_iterator );
            if ( existing_definition->hash_value == new_definition->hash_value
                 && existing_definition->group_handle == new_definition->group_handle
                 && existing_definition->parent_handle == new_definition->parent_handle
                 && existing_definition->unify_key == new_definition->unify_key )
            {
                /* This is a duplicate, take the name of the new definition, if any */
                if ( new_definition->name_handle != SCOREP_INVALID_STRING )
                {
                    existing_definition->name_handle = new_definition->name_handle;
                }
                SCOREP_Allocator_RollbackAllocMovable(
                    definition_manager->page_manager,
                    new_handle );
                return hash_list_iterator;
            }
            hash_list_iterator = existing_definition->hash_next;
        }
        new_definition->hash_next = *hash_table_bucket;
        *hash_table_bucket        = new_handle;
    }
    *comms->tail                    = new_handle;
    comms->tail                     = &new_definition->next;
    new_definition->sequence_number = comms->counter++;

    /* no NewDefinitionHandle, communicators should only be defined at unification */

    return new_handle;
}
