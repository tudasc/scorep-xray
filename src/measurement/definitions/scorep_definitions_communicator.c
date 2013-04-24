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
 * @file       src/measurement/definitions/scorep_definitions_communicator.c
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


static SCOREP_InterimCommunicatorHandle
define_interim_communicator( SCOREP_DefinitionManager*        definition_manager,
                             SCOREP_InterimCommunicatorHandle parentComm,
                             SCOREP_AdapterType               adapterType,
                             size_t                           sizeOfPayload,
                             void**                           payload );

static bool
equal_interim_communicator( const SCOREP_InterimCommunicatorDef* existingDefinition,
                            const SCOREP_InterimCommunicatorDef* newDefinition );

static size_t
local_comm_static_size()
{
    return SCOREP_Allocator_RoundupToAlignment(
               sizeof( SCOREP_InterimCommunicatorDef ) );
}

/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicator( SCOREP_InterimCommunicatorHandle parentComm,
                                           SCOREP_AdapterType               adapterType,
                                           size_t                           sizeOfPayload,
                                           void**                           payload )
{
    SCOREP_InterimCommunicatorHandle new_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    new_handle = define_interim_communicator(
        &scorep_local_definition_manager,
        parentComm,
        adapterType,
        sizeOfPayload,
        payload );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void*
SCOREP_InterimCommunicatorHandle_GetPayload( SCOREP_InterimCommunicatorHandle handle )
{
    return ( char* )SCOREP_LOCAL_HANDLE_DEREF( handle,
                                               InterimCommunicator ) + local_comm_static_size();
}


void
SCOREP_InterimCommunicatorHandle_SetName( SCOREP_InterimCommunicatorHandle localMPICommHandle,
                                          const char*                      name )
{
    UTILS_BUG_ON( localMPICommHandle == SCOREP_INVALID_INTERIM_COMMUNICATOR,
                  "Invalid MPI_Comm handle as argument" );

    SCOREP_Definitions_Lock();

    SCOREP_InterimCommunicatorDef* definition = SCOREP_LOCAL_HANDLE_DEREF(
        localMPICommHandle,
        InterimCommunicator );

    if ( definition->name_handle == SCOREP_INVALID_STRING )
    {
        definition->name_handle = scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "" );
    }

    SCOREP_Definitions_Unlock();
}


static SCOREP_InterimCommunicatorHandle
define_interim_communicator( SCOREP_DefinitionManager*        definition_manager,
                             SCOREP_InterimCommunicatorHandle parentComm,
                             SCOREP_AdapterType               adapterType,
                             size_t                           sizeOfPayload,
                             void**                           payload )
{
    SCOREP_InterimCommunicatorDef*   new_definition = NULL;
    SCOREP_InterimCommunicatorHandle new_handle     = SCOREP_INVALID_INTERIM_COMMUNICATOR;

    size_t payload_offset = local_comm_static_size();
    size_t total_size     = payload_offset + sizeOfPayload;
    SCOREP_DEFINITION_ALLOC_SIZE( InterimCommunicator, total_size );

    // Init new_definition
    new_definition->name_handle   = SCOREP_INVALID_STRING;
    new_definition->parent_handle = parentComm;
    new_definition->adapter_type  = adapterType;

    UTILS_BUG_ON( definition_manager->interim_communicator_definition_hash_table,
                  "local communicator definitions shouldn't have a hash table" );
    /* Does never return, because they will never be automatically unified */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( InterimCommunicator,
                                              interim_communicator );

    if ( sizeOfPayload && payload )
    {
        *payload = ( char* )new_definition + payload_offset;
    }

    return new_handle;
}

bool
equal_interim_communicator( const SCOREP_InterimCommunicatorDef* existingDefinition,
                            const SCOREP_InterimCommunicatorDef* newDefinition )
{
    return false;
}

static bool
equal_communicator( const SCOREP_CommunicatorDef* existingDefinition,
                    const SCOREP_CommunicatorDef* newDefinition );


static SCOREP_CommunicatorHandle
define_communicator( SCOREP_DefinitionManager* definition_manager,
                     SCOREP_GroupHandle        group_handle,
                     SCOREP_StringHandle       name_handle,
                     SCOREP_CommunicatorHandle parent_handle );


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
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
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "<unnamed communicator>" ),
        parent_handle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
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
               scorep_string_definition_define(
                   scorep_unified_definition_manager,
                   name ? name : "<unnamed communicator>" ),
               parent_handle );
}

void
SCOREP_CopyCommunicatorDefinitionToUnified( SCOREP_CommunicatorDef*       definition,
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
        SCOREP_HANDLE_GET_UNIFIED( definition->group_handle, Group, handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
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
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Communicator,
                                              communicator );

    return new_handle;
}


bool
equal_communicator( const SCOREP_CommunicatorDef* existingDefinition,
                    const SCOREP_CommunicatorDef* newDefinition )
{
    return false;
}
