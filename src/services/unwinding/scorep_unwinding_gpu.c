/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_gpu.h"

#include <SCOREP_Unwinding.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>

#include <unistd.h>
#include <string.h>

#include "scorep_unwinding_region.h"

SCOREP_Unwinding_GpuLocationData*
scorep_unwinding_gpu_get_location_data( SCOREP_Location* location )
{
    /* Create per-location unwinding management data for non-CPU locations */
    SCOREP_Unwinding_GpuLocationData* gpu_unwind_data =
        SCOREP_Location_AllocForMisc( location,
                                      sizeof( *gpu_unwind_data ) );

    /* Initialize the object */
    memset( gpu_unwind_data, 0, sizeof( *gpu_unwind_data ) );
    gpu_unwind_data->location                 = location;
    gpu_unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;

    return gpu_unwind_data;
}

/**
 * Pushes an instrumented function to the list of instrumented functions.
 *
 * @param unwindData        Unwind data.
 * @param regionHandle      The region handle for the instrumented region
 *
 * @return Pointer to new entry representing an instrumented function
 */
static scorep_unwinding_instrumented_region*
push_instrumented_region( SCOREP_Unwinding_GpuLocationData* unwindData,
                          SCOREP_RegionHandle               regionHandle )
{
    scorep_unwinding_instrumented_region* instrumented_region = unwindData->unused_instrumented_regions;
    if ( instrumented_region )
    {
        unwindData->unused_instrumented_regions = instrumented_region->prev;
    }
    else
    {
        instrumented_region = SCOREP_Location_AllocForMisc( unwindData->location,
                                                            sizeof( *instrumented_region ) );
    }
    memset( instrumented_region, 0, sizeof( *instrumented_region ) );

    instrumented_region->region_handle = regionHandle;

    instrumented_region->prev        = unwindData->instrumented_regions;
    unwindData->instrumented_regions = instrumented_region;

    return instrumented_region;
}

/**
 * Pops entry from the list of instrumented functions.
 *
 * @param unwindData        Unwind data.
 */
static void
pop_instrumented_region( SCOREP_Unwinding_GpuLocationData* unwindData )
{
    scorep_unwinding_instrumented_region* top = unwindData->instrumented_regions;
    unwindData->instrumented_regions        = top->prev;
    top->prev                               = unwindData->unused_instrumented_regions;
    unwindData->unused_instrumented_regions = top;
}

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_enter( SCOREP_Unwinding_GpuLocationData* unwindData,
                                   SCOREP_RegionHandle               instrumentedRegionHandle,
                                   SCOREP_CallingContextHandle*      callingContext,
                                   uint32_t*                         unwindDistance,
                                   SCOREP_CallingContextHandle*      previousCallingContext )
{
    UTILS_DEBUG_ENTRY( "instrumentedRegionHandle=%u[%s]",
                       instrumentedRegionHandle,
                       instrumentedRegionHandle
                       ? SCOREP_RegionHandle_GetName( instrumentedRegionHandle )
                       : "" );

    if ( !unwindData )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    *previousCallingContext = unwindData->previous_calling_context;

    UTILS_BUG_ON( instrumentedRegionHandle == SCOREP_INVALID_REGION, "GPU enter called without instrumented region handle" );

    /*
     * A valid instrumentedRegionHandle indicates that this enter
     * event was triggered by an instrumented function. The list of
     * instrumented functions within the current stack is maintained
     * in unwindData->instrumented_regions and needs to be updated.
     */
    scorep_unwinding_instrumented_region* instrumented_region = push_instrumented_region( unwindData,
                                                                                          instrumentedRegionHandle );

    scorep_unwinding_calling_context_tree_node* current;
    scorep_unwinding_calling_context_tree_node* child;
    bool                                        child_found = false;

    /* Update calling context tree */
    if ( instrumented_region->prev == NULL )
    {
        /* Instrumented region on first stack level, it has no predecessor */
        current = &unwindData->calling_context_root;
    }
    else
    {
        /* Get instrumented region of the stack level above to search in its children array */
        current = instrumented_region->prev->cct_node;
    }

    /* Search children array to check whether current region
     * (instrumentedRegionHandle) was already visited before,
     * if yes reuse the corresponding child, otherwise
     * create a new child in the calling context tree */
    child = current->children;
    while ( child && !child_found )
    {
        if ( child->region == instrumentedRegionHandle && child->ip == 0 )
        {
            /*
             * If found, use this child as new parent and go to
             * next element in the instruction stack
             */
            current     = child;
            child_found = true;
        }
        child = child->next_sibling;
    }

    /* If not found create a new calling context entry */
    if ( !child_found )
    {
        /* Allocate memory for a new child */
        scorep_unwinding_calling_context_tree_node* new_child =
            SCOREP_Location_AllocForMisc( unwindData->location, sizeof( *new_child ) );

        new_child->children = NULL;
        new_child->ip       = 0;
        new_child->region   = instrumentedRegionHandle;
        new_child->handle   =
            SCOREP_Definitions_NewCallingContext( 0,
                                                  new_child->region,
                                                  SCOREP_INVALID_SOURCE_CODE_LOCATION,
                                                  current->handle );

        /* Enqueue new child to parent's list of children */
        new_child->next_sibling = current->children;
        current->children       = new_child;

        /* The new allocated child is our new node now */
        current = new_child;
    }

    instrumented_region->cct_node        = current;
    *callingContext                      = current->handle;
    unwindData->previous_calling_context = *callingContext;
    *unwindDistance                      = 2;

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_exit( SCOREP_Unwinding_GpuLocationData* unwindData,
                                  SCOREP_CallingContextHandle*      callingContext,
                                  uint32_t*                         unwindDistance,
                                  SCOREP_CallingContextHandle*      previousCallingContext )
{
    if ( !unwindData )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    *previousCallingContext = unwindData->previous_calling_context;

    UTILS_DEBUG_ENTRY( "%p", unwindData->location );

    scorep_unwinding_instrumented_region* instrumented_region = unwindData->instrumented_regions;

    /* Update calling context tree */
    *unwindDistance = 1;
    *callingContext = *previousCallingContext;

    if ( instrumented_region->prev == NULL )
    {
        unwindData->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;
    }
    else
    {
        unwindData->previous_calling_context = SCOREP_CallingContextHandle_GetParent( *callingContext );
    }

    /* Remove element from instrumented instructions */
    pop_instrumented_region( unwindData );

    return SCOREP_SUCCESS;
}
