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

/**
 * Element in the list of instrumented regions.
 */
typedef struct scorep_unwinding_instrumented_region
{
    /** Corresponding region handle */
    SCOREP_RegionHandle                          region_handle;
    /** Pointer to corresponding node in the calling context tree */
    scorep_unwinding_calling_context_tree_node*  cct_node;
    /** Pointer to previous instrumented function in the stack */
    struct scorep_unwinding_instrumented_region* prev;
} scorep_unwinding_instrumented_region;

/** Per-location based data related to unwinding for all GPU locations. */
typedef struct SCOREP_Unwinding_GpuLocationData
{
    /** List of regions that were entered through instrumentation */
    scorep_unwinding_instrumented_region* instrumented_regions;
    /** List with free elements for the inst_list */
    scorep_unwinding_instrumented_region* unused_instrumented_regions;
    /** Root calling context node for all generated nodes,
        i.e., the SCOREP_INVALID_CALLING_CONTEXT node. */
    scorep_unwinding_calling_context_tree_node calling_context_root;
    /** Recently visited calling context leaf node. */
    scorep_unwinding_calling_context_tree_node calling_context_current_leaf;
    /** Last known calling context */
    SCOREP_CallingContextHandle                previous_calling_context;
} SCOREP_Unwinding_GpuLocationData;


SCOREP_ErrorCode
scorep_unwinding_gpu_init_location( SCOREP_Location* location )
{
    /* Create per-location unwinding management data for non-CPU locations */
    SCOREP_Unwinding_GpuLocationData* gpu_unwind_data =
        SCOREP_Location_AllocForMisc( location,
                                      sizeof( *gpu_unwind_data ) );

    /* Initialize the object */
    memset( gpu_unwind_data, 0, sizeof( *gpu_unwind_data ) );
    gpu_unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;

    SCOREP_Location_SetSubsystemData( location,
                                      scorep_unwinding_subsystem_id,
                                      gpu_unwind_data );

    return SCOREP_SUCCESS;
}

/**
 * Pushes an instrumented function to the list of instrumented functions.
 *
 * @param[inout] instrumentedRegionList     List of regions that were entered through instrumentation
 * @param[inout] instrumentedRegionFreeList List with free elements for instrumented regions
 * @param instruction                       The instruction object for the instrumented region
 * @param regionHandle                      The region handle for the instrumented region
 *
 * @return Pointer to new entry representing an instrumented function
 */
static scorep_unwinding_instrumented_region*
push_instrumented_region( scorep_unwinding_instrumented_region** instrumentedRegionList,
                          scorep_unwinding_instrumented_region** instrumentedRegionFreeList,
                          SCOREP_RegionHandle                    regionHandle )
{
    scorep_unwinding_instrumented_region* instrumented_region = *instrumentedRegionFreeList;
    if ( instrumented_region )
    {
        *instrumentedRegionFreeList = instrumented_region->prev;
    }
    else
    {
        instrumented_region = SCOREP_Memory_AllocForMisc( sizeof( *instrumented_region ) );
    }
    memset( instrumented_region, 0, sizeof( *instrumented_region ) );

    instrumented_region->region_handle = regionHandle;

    instrumented_region->prev = *instrumentedRegionList;
    *instrumentedRegionList   = instrumented_region;

    return instrumented_region;
}

/**
 * Pops entry from the list of instrumented functions.
 *
 * @param[inout] instrumentedRegionList     List of regions that were entered through instrumentation
 * @param[inout] instrumentedRegionFreeList List with free elements for instrumented regions
 */
static void
pop_instrumented_region( scorep_unwinding_instrumented_region** instrumentedRegionList,
                         scorep_unwinding_instrumented_region** instrumentedRegionFreeList )
{
    scorep_unwinding_instrumented_region* top = *instrumentedRegionList;
    *instrumentedRegionList     = top->prev;
    top->prev                   = *instrumentedRegionFreeList;
    *instrumentedRegionFreeList = top;
}

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_enter( SCOREP_Location*             location,
                                   SCOREP_RegionHandle          instrumentedRegionHandle,
                                   SCOREP_CallingContextHandle* callingContext,
                                   uint32_t*                    unwindDistance,
                                   SCOREP_CallingContextHandle* previousCallingContext )
{
    UTILS_DEBUG_ENTRY( "instrumentedRegionHandle=%u[%s]",
                       instrumentedRegionHandle,
                       instrumentedRegionHandle
                       ? SCOREP_RegionHandle_GetName( instrumentedRegionHandle )
                       : "" );

    SCOREP_Unwinding_GpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    if ( !unwind_data )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    *previousCallingContext = unwind_data->previous_calling_context;

    UTILS_BUG_ON( instrumentedRegionHandle == SCOREP_INVALID_REGION, "GPU enter called without instrumented region handle" );

    /*
     * A valid instrumentedRegionHandle indicates that this enter
     * event was triggered by an instrumented function. The list of
     * instrumented functions within the current stack is maintained
     * in unwind_data->instrumented_regions and needs to be updated.
     */
    scorep_unwinding_instrumented_region* instrumented_region = push_instrumented_region( &unwind_data->instrumented_regions,
                                                                                          &unwind_data->unused_instrumented_regions,
                                                                                          instrumentedRegionHandle );

    scorep_unwinding_calling_context_tree_node* current;
    scorep_unwinding_calling_context_tree_node* child;
    bool                                        child_found = false;

    /* Update calling context tree */
    if ( instrumented_region->prev == NULL )
    {
        /* Instrumented region on first stack level, it has no predecessor */
        current = &unwind_data->calling_context_root;
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
            SCOREP_Location_AllocForMisc( location, sizeof( *new_child ) );

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

    instrumented_region->cct_node         = current;
    *callingContext                       = current->handle;
    unwind_data->previous_calling_context = *callingContext;
    *unwindDistance                       = 2;

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_exit( SCOREP_Location*             location,
                                  SCOREP_CallingContextHandle* callingContext,
                                  uint32_t*                    unwindDistance,
                                  SCOREP_CallingContextHandle* previousCallingContext )
{
    SCOREP_Unwinding_GpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    if ( !unwind_data )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "location has no unwind data?" );
    }

    *previousCallingContext = unwind_data->previous_calling_context;

    UTILS_DEBUG_ENTRY( "%p", location );

    scorep_unwinding_instrumented_region* instrumented_region = unwind_data->instrumented_regions;

    /* Update calling context tree */
    *unwindDistance = 1;
    *callingContext = *previousCallingContext;

    if ( instrumented_region->prev == NULL )
    {
        unwind_data->previous_calling_context = SCOREP_INVALID_CALLING_CONTEXT;
    }
    else
    {
        unwind_data->previous_calling_context = SCOREP_CallingContextHandle_GetParent( *callingContext );
    }

    /* Remove element from instrumented instructions */
    pop_instrumented_region( &unwind_data->instrumented_regions,
                             &unwind_data->unused_instrumented_regions );

    return SCOREP_SUCCESS;
}
