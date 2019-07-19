/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2019,
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
 * Copyright (c) 2009-2014, 2017,
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
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_RmaWindowHandle
define_rma_window( SCOREP_DefinitionManager* definition_manager,
                   SCOREP_StringHandle       nameHandle,
                   SCOREP_CommunicatorHandle communicatorHandle,
                   uint32_t                  creationId );


static void
hash_rma_window( SCOREP_RmaWindowDef* definition );

/**
 * Associate the parameter tuple with a process unique RMA window handle.
 */
SCOREP_RmaWindowHandle
SCOREP_Definitions_NewRmaWindow( const char*                      name,
                                 SCOREP_InterimCommunicatorHandle communicatorHandle )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_RmaWindowHandle new_handle = define_rma_window(
        &scorep_local_definition_manager,
        name ? scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name, NULL ) : SCOREP_INVALID_STRING,
        communicatorHandle,
        scorep_definitions_interim_communicator_get_rma_window_creation_counter( communicatorHandle ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_RmaWindowHandle_SetName( SCOREP_RmaWindowHandle rmaWindowHandle,
                                const char*            name )
{
    UTILS_BUG_ON( rmaWindowHandle == SCOREP_INVALID_RMA_WINDOW,
                  "Invalid RmaWindow handle as argument" );

    SCOREP_Definitions_Lock();

    SCOREP_RmaWindowDef* definition = SCOREP_LOCAL_HANDLE_DEREF(
        rmaWindowHandle,
        RmaWindow );

    /**
     * Note: The MPI adapter provides four distinct default names for the four
     * window creation routines. The unification does not use the name for the
     * equality test, and users are allowed to provide a specific name only on
     * one of the processes with the handle, or even different names on
     * different processes. In these cases, the name handle after unification
     * is 'undefined' from a user's perspective. The current unification scheme
     * considers the first user-defined name set on the first process (in rank
     * order) of the corrsponding communicator. If no name is set by the user,
     * the default name (used at definition creation) will be used instead.
     */
    if ( definition->has_default_name )
    {
        definition->name_handle = scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL );
        definition->has_default_name = false;
    }

    SCOREP_Definitions_Unlock();
}


void
scorep_definitions_unify_rma_window( SCOREP_RmaWindowDef*          definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_StringHandle unified_name_handle = SCOREP_INVALID_STRING;
    if ( definition->name_handle != SCOREP_INVALID_STRING )
    {
        unified_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_name_handle == SCOREP_INVALID_STRING,
                      "RMA window name not yet unified." );
    }

    definition->unified = define_rma_window(
        scorep_unified_definition_manager,
        unified_name_handle,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->communicator_handle,
            Communicator,
            handlesPageManager ),
        definition->creation_id );
}


void
scorep_definitions_rehash_rma_window( SCOREP_RmaWindowDef* definition )
{
    definition->hash_value = 0;

    hash_rma_window( definition );
}

SCOREP_RmaWindowHandle
define_rma_window( SCOREP_DefinitionManager* definition_manager,
                   SCOREP_StringHandle       nameHandle,
                   SCOREP_CommunicatorHandle communicatorHandle,
                   uint32_t                  creationId )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_RmaWindowDef*   new_definition = NULL;
    SCOREP_RmaWindowHandle new_handle     = SCOREP_INVALID_RMA_WINDOW;

    SCOREP_DEFINITION_ALLOC( RmaWindow );

    new_definition->name_handle         = nameHandle;
    new_definition->communicator_handle = communicatorHandle;
    new_definition->creation_id         = creationId;
    new_definition->has_default_name    = true;

    hash_rma_window( new_definition );

    /* Open coded SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION, to handle the
     * rma window name */
    scorep_definitions_manager_entry* rma_windows = &definition_manager->rma_window;
    if ( rma_windows->hash_table )
    {
        SCOREP_RmaWindowHandle* hash_table_bucket =
            &rma_windows->hash_table[
                new_definition->hash_value & rma_windows->hash_table_mask ];
        SCOREP_RmaWindowHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SCOREP_INVALID_RMA_WINDOW )
        {
            SCOREP_RmaWindowDef* existing_definition =
                SCOREP_Allocator_GetAddressFromMovableMemory(
                    definition_manager->page_manager,
                    hash_list_iterator );
            if ( existing_definition->hash_value == new_definition->hash_value
                 && existing_definition->communicator_handle == new_definition->communicator_handle
                 && existing_definition->creation_id == new_definition->creation_id )
            {
                /* This is a duplicate, but only take the name of the new definition
                 * if the existing handle has not yet been specifically named */
                if ( existing_definition->has_default_name
                     && new_definition->name_handle != SCOREP_INVALID_STRING )
                {
                    existing_definition->name_handle      = new_definition->name_handle;
                    existing_definition->has_default_name = false;
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
    *rma_windows->tail              = new_handle;
    rma_windows->tail               = &new_definition->next;
    new_definition->sequence_number = rma_windows->counter++;

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_RMA_WINDOW ) );
    }

    return new_handle;
}

void
hash_rma_window( SCOREP_RmaWindowDef* definition )
{
    HASH_ADD_HANDLE( definition, communicator_handle, Communicator );
    HASH_ADD_POD( definition, creation_id );
}
