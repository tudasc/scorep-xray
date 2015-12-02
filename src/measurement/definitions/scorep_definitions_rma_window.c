/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
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
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_InterimRmaWindowHandle
define_interim_rma_window( SCOREP_DefinitionManager*        definition_manager,
                           SCOREP_StringHandle              nameHandle,
                           SCOREP_InterimCommunicatorHandle communicatorHandle );


static bool
equal_interim_rma_window( const SCOREP_InterimRmaWindowDef* existingDefinition,
                          const SCOREP_InterimRmaWindowDef* newDefinition );


/**
 * Associate the parameter tuple with a process unique RMA window handle.
 */
SCOREP_InterimRmaWindowHandle
SCOREP_Definitions_NewInterimRmaWindow( const char*                      name,
                                        SCOREP_InterimCommunicatorHandle communicatorHandle )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_InterimRmaWindowHandle new_handle = define_interim_rma_window(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown RMA window>", NULL ),
        communicatorHandle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_InterimRmaWindowHandle
define_interim_rma_window( SCOREP_DefinitionManager*        definition_manager,
                           SCOREP_StringHandle              nameHandle,
                           SCOREP_InterimCommunicatorHandle communicatorHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_InterimRmaWindowDef*   new_definition = NULL;
    SCOREP_InterimRmaWindowHandle new_handle     = SCOREP_INVALID_INTERIM_RMA_WINDOW;

    SCOREP_DEFINITION_ALLOC( InterimRmaWindow );

    new_definition->name_handle         = nameHandle;
    new_definition->communicator_handle = communicatorHandle;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( InterimRmaWindow, interim_rma_window );

    return new_handle;
}


bool
equal_interim_rma_window( const SCOREP_InterimRmaWindowDef* existingDefinition,
                          const SCOREP_InterimRmaWindowDef* newDefinition )
{
    return false;
}


static SCOREP_RmaWindowHandle
define_rma_window( SCOREP_DefinitionManager*        definition_manager,
                   SCOREP_StringHandle              nameHandle,
                   SCOREP_InterimCommunicatorHandle communicatorHandle );


static bool
equal_rma_window( const SCOREP_RmaWindowDef* existingDefinition,
                  const SCOREP_RmaWindowDef* newDefinition );


/**
 * Associate the parameter tuple with a process unique RMA window handle.
 */
SCOREP_RmaWindowHandle
SCOREP_Definitions_NewRmaWindow( const char*               name,
                                 SCOREP_CommunicatorHandle communicatorHandle )
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


SCOREP_RmaWindowHandle
SCOREP_Definitions_NewUnifiedRmaWindow( const char*               name,
                                        SCOREP_CommunicatorHandle communicatorHandle )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    return define_rma_window(
        scorep_unified_definition_manager,
        scorep_definitions_new_string(
            scorep_unified_definition_manager,
            name ? name : "<unknown RMA window>", NULL ),
        communicatorHandle );
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

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( RmaWindow, rma_window );

    return new_handle;
}


bool
equal_rma_window( const SCOREP_RmaWindowDef* existingDefinition,
                  const SCOREP_RmaWindowDef* newDefinition )
{
    return false;
}
