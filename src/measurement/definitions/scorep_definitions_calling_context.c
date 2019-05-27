/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
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


static SCOREP_CallingContextHandle
define_calling_context( SCOREP_DefinitionManager*       definition_manager,
                        uint64_t                        ip,
                        uint64_t                        ipOffset,
                        SCOREP_StringHandle             file,
                        SCOREP_RegionHandle             region,
                        SCOREP_SourceCodeLocationHandle scl,
                        SCOREP_CallingContextHandle     parent );


static bool
equal_calling_context( const SCOREP_CallingContextDef* existingDefinition,
                       const SCOREP_CallingContextDef* newDefinition );


SCOREP_CallingContextHandle
SCOREP_Definitions_NewCallingContext( uint64_t                        ip,
                                      SCOREP_RegionHandle             region,
                                      SCOREP_SourceCodeLocationHandle scl,
                                      SCOREP_CallingContextHandle     parent )
{
    UTILS_DEBUG_ENTRY( "ip %" PRIx64 ", region %u, scl %u, parent %u",
                       ip, region, scl, parent );

    SCOREP_Definitions_Lock();

    SCOREP_CallingContextHandle new_handle = define_calling_context(
        &scorep_local_definition_manager,
        ip,
        0,
        SCOREP_INVALID_STRING,
        region,
        scl,
        parent );

    SCOREP_Definitions_Unlock();


    UTILS_DEBUG_EXIT( "ip %" PRIx64 ", region %u, scl %u, parent %u: %u",
                      ip, region, scl, parent, new_handle );
    return new_handle;
}

SCOREP_RegionHandle
SCOREP_CallingContextHandle_GetRegion( SCOREP_CallingContextHandle handle )
{
    SCOREP_CallingContextDef* calling_context = SCOREP_LOCAL_HANDLE_DEREF( handle, CallingContext );
    return calling_context->region_handle;
}

SCOREP_CallingContextHandle
SCOREP_CallingContextHandle_GetParent( SCOREP_CallingContextHandle handle )
{
    SCOREP_CallingContextDef* calling_context = SCOREP_LOCAL_HANDLE_DEREF( handle, CallingContext );
    return calling_context->parent_handle;
}


void
scorep_definitions_unify_calling_context( SCOREP_CallingContextDef*     definition,
                                          SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_StringHandle unified_file_handle = SCOREP_INVALID_STRING;
    if ( definition->file_handle != SCOREP_INVALID_STRING )
    {
        unified_file_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->file_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_file_handle == SCOREP_INVALID_STRING,
                      "Invalid unification order of calling context definition: file not yet unified" );
    }

    SCOREP_SourceCodeLocationHandle unified_scl_handle = SCOREP_INVALID_SOURCE_CODE_LOCATION;
    if ( definition->scl_handle != SCOREP_INVALID_SOURCE_CODE_LOCATION )
    {
        unified_scl_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->scl_handle,
            SourceCodeLocation,
            handlesPageManager );
        UTILS_BUG_ON( unified_scl_handle == SCOREP_INVALID_SOURCE_CODE_LOCATION,
                      "Invalid unification order of calling context definition: scl not yet unified" );
    }

    SCOREP_CallingContextHandle unified_parent_handle = SCOREP_INVALID_CALLING_CONTEXT;
    if ( definition->parent_handle != SCOREP_INVALID_CALLING_CONTEXT )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            CallingContext,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_CALLING_CONTEXT,
                      "Invalid unification order of calling context definition: parent not yet unified" );
    }

    definition->unified = define_calling_context(
        scorep_unified_definition_manager,
        definition->ip,
        definition->ip_offset,
        unified_file_handle,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->region_handle,
            Region,
            handlesPageManager ),
        unified_scl_handle,
        unified_parent_handle );
}


bool
equal_calling_context( const SCOREP_CallingContextDef* existingDefinition,
                       const SCOREP_CallingContextDef* newDefinition )
{
    return existingDefinition->ip_offset     == newDefinition->ip_offset &&
           existingDefinition->file_handle   == newDefinition->file_handle &&
           existingDefinition->region_handle == newDefinition->region_handle &&
           existingDefinition->scl_handle    == newDefinition->scl_handle &&
           existingDefinition->parent_handle == newDefinition->parent_handle;
}


SCOREP_CallingContextHandle
define_calling_context( SCOREP_DefinitionManager*       definition_manager,
                        uint64_t                        ip,
                        uint64_t                        ipOffset,
                        SCOREP_StringHandle             file,
                        SCOREP_RegionHandle             region,
                        SCOREP_SourceCodeLocationHandle scl,
                        SCOREP_CallingContextHandle     parent )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_CallingContextDef*   new_definition = NULL;
    SCOREP_CallingContextHandle new_handle     = SCOREP_INVALID_CALLING_CONTEXT;

    SCOREP_DEFINITION_ALLOC( CallingContext );

    new_definition->ip = ip;

    new_definition->ip_offset = ipOffset;
    HASH_ADD_POD( new_definition, ip_offset );

    new_definition->file_handle = file;
    if ( new_definition->file_handle != SCOREP_INVALID_STRING )
    {
        HASH_ADD_HANDLE( new_definition, file_handle, String );
    }

    new_definition->region_handle = region;
    HASH_ADD_HANDLE( new_definition, region_handle, Region );

    new_definition->scl_handle = scl;
    if ( new_definition->scl_handle != SCOREP_INVALID_SOURCE_CODE_LOCATION )
    {
        HASH_ADD_HANDLE( new_definition, scl_handle, SourceCodeLocation );
    }

    new_definition->parent_handle = parent;
    if ( new_definition->parent_handle != SCOREP_INVALID_CALLING_CONTEXT )
    {
        HASH_ADD_HANDLE( new_definition, parent_handle, CallingContext );
    }

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( CallingContext, calling_context );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_CALLING_CONTEXT ) );
    }

    return new_handle;
}

static SCOREP_InterruptGeneratorHandle
define_interrupt_generator( SCOREP_DefinitionManager*     definition_manager,
                            SCOREP_StringHandle           name_handle,
                            SCOREP_InterruptGeneratorMode mode,
                            SCOREP_MetricBase             base,
                            int64_t                       exponent,
                            uint64_t                      period );


static bool
equal_interrupt_generator( const SCOREP_InterruptGeneratorDef* existingDefinition,
                           const SCOREP_InterruptGeneratorDef* newDefinition );


SCOREP_InterruptGeneratorHandle
SCOREP_Definitions_NewInterruptGenerator( const char*                   name,
                                          SCOREP_InterruptGeneratorMode mode,
                                          SCOREP_MetricBase             base,
                                          int64_t                       exponent,
                                          uint64_t                      period )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_InterruptGeneratorHandle new_handle = define_interrupt_generator(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown interrupt generator>", NULL ),
        mode,
        base,
        exponent,
        period );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_interrupt_generator( SCOREP_InterruptGeneratorDef* definition,
                                              SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_interrupt_generator(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->mode,
        definition->base,
        definition->exponent,
        definition->period );
}


bool
equal_interrupt_generator( const SCOREP_InterruptGeneratorDef* existingDefinition,
                           const SCOREP_InterruptGeneratorDef* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle &&
           existingDefinition->mode        == newDefinition->mode &&
           existingDefinition->base        == newDefinition->base &&
           existingDefinition->exponent    == newDefinition->exponent &&
           existingDefinition->period      == newDefinition->period;
}


SCOREP_InterruptGeneratorHandle
define_interrupt_generator( SCOREP_DefinitionManager*     definition_manager,
                            SCOREP_StringHandle           name_handle,
                            SCOREP_InterruptGeneratorMode mode,
                            SCOREP_MetricBase             base,
                            int64_t                       exponent,
                            uint64_t                      period )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_InterruptGeneratorDef*   new_definition = NULL;
    SCOREP_InterruptGeneratorHandle new_handle     = SCOREP_INVALID_CALLING_CONTEXT;

    SCOREP_DEFINITION_ALLOC( InterruptGenerator );

    new_definition->name_handle = name_handle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->mode = mode;
    HASH_ADD_POD( new_definition, mode );

    new_definition->base = base;
    HASH_ADD_POD( new_definition, base );

    new_definition->exponent = exponent;
    HASH_ADD_POD( new_definition, exponent );

    new_definition->period = period;
    HASH_ADD_POD( new_definition, period );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( InterruptGenerator, interrupt_generator );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_INTERRUPT_GENERATOR ) );
    }

    return new_handle;
}
