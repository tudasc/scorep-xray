/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
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


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_RmaWindowHandle
define_rma_window( SCOREP_DefinitionManager* definition_manager,
                   SCOREP_StringHandle       nameHandle,
                   SCOREP_CommunicatorHandle communicatorHandle );


static bool
equal_rma_window( const SCOREP_RmaWindowDef* existingDefinition,
                  const SCOREP_RmaWindowDef* newDefinition );


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
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown RMA window>", NULL ),
        communicatorHandle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_rma_window( SCOREP_RmaWindowDef*          definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_rma_window(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->communicator_handle,
            Communicator,
            handlesPageManager ) );
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
                   SCOREP_CommunicatorHandle communicatorHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_RmaWindowDef*   new_definition = NULL;
    SCOREP_RmaWindowHandle new_handle     = SCOREP_INVALID_RMA_WINDOW;

    SCOREP_DEFINITION_ALLOC( RmaWindow );

    new_definition->name_handle         = nameHandle;
    new_definition->communicator_handle = communicatorHandle;

    hash_rma_window( new_definition );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( RmaWindow, rma_window );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_RMA_WINDOW ) );
    }

    return new_handle;
}


bool
equal_rma_window( const SCOREP_RmaWindowDef* existingDefinition,
                  const SCOREP_RmaWindowDef* newDefinition )
{
    return existingDefinition->communicator_handle == newDefinition->communicator_handle;
}

void
hash_rma_window( SCOREP_RmaWindowDef* definition )
{
    HASH_ADD_HANDLE( definition, communicator_handle, Communicator );
}
