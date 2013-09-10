/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>
#include "SCOREP_Pomp_Common.h"

#include "SCOREP_Pomp_RegionInfo.h"
#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>
#include <SCOREP_Subsystem.h>

#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <opari2/pomp2_lib.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


/* *************************************************************************
                                                            Declarations
 ****************************************************************************/

/** Pointer to an array of all pomp regions */
SCOREP_Pomp_Region* scorep_pomp_regions;

/** Flag to indicate, whether POMP2 traceing is enable/disabled */
bool scorep_pomp_is_tracing_on = true;

/** Flag to indicate whether the adapter is initialized */
bool scorep_pomp_is_initialized = false;

/** Flag to indicate whether the adapter is finalized */
bool scorep_pomp_is_finalized = false;

/** Lock to protect on-the-fly assignments.*/
SCOREP_Mutex scorep_pomp_assign_lock;

/** List elements of regions that are registered on the fly */
typedef struct scorep_stack_node scorep_stack_node;

struct scorep_stack_node
{
    scorep_stack_node* next;
    SCOREP_Pomp_Region region;
};

/** List of regions that are registered on the fly */
static scorep_stack_node* scorep_region_list_top = NULL;

/* *********************************************************************
                                                    Internal functions
 ************************************************************************/

static SCOREP_Pomp_Region*
scorep_pomp_allocate_region_on_the_fly()
{
    scorep_stack_node* node = malloc( sizeof( scorep_stack_node ) );
    node->next             = scorep_region_list_top;
    scorep_region_list_top = node;
    return &node->region;
}

/** Frees allocated memory and sets the pointer to 0. Used to free
    members of a SCOREP_Pomp_Region instance.
    @param member Pointer to the pointer which should be freed. If
                  member is 0, nothing happens.
 */
static void
scorep_pomp_free_region_member( char** member )
{
    if ( *member )
    {
        free( *member );
        *member = 0;
    }
}

/** Frees allocated memory of all members of a SCOREP_Pomp_Region
    instance.
    @param region The region for which all members should be freed. If
                  region is 0, nothing happens.
 */
static void
scorep_pomp_free_region_members( SCOREP_Pomp_Region* region )
{
    if ( region )
    {
        scorep_pomp_free_region_member( &region->name );
        scorep_pomp_free_region_member( &region->startFileName );
        scorep_pomp_free_region_member( &region->endFileName );
    }
}

/** Copys a string to a given position. Allocates the necessary space for the
    destination.
    @param destination Pointer to the string where the string is copied into.
    @param source      String which is copied.
 */
static void
scorep_pomp_assign_string( char**      destination,
                           const char* source )
{
    UTILS_ASSERT( source );
    *destination = malloc( strlen( source ) * sizeof( char ) + 1 );
    strcpy( *destination, source );
}

/* *********************************************************************
                                                       Initialization
 ************************************************************************/

/** Adapter initialization function.
 */
SCOREP_ErrorCode
scorep_pomp_adapter_init( void )
{
    UTILS_DEBUG_ENTRY();
    /* Initialize the adapter */
    if ( !scorep_pomp_is_initialized )
    {
        /* Set flag */
        scorep_pomp_is_initialized = true;

        /* Allocate memory for your POMP2_Get_num_regions() regions */
        scorep_pomp_regions = calloc( POMP2_Get_num_regions(),
                                      sizeof( SCOREP_Pomp_Region ) );

        SCOREP_MutexCreate( &scorep_pomp_assign_lock );

        scorep_pomp_lock_initialize();

        /* Register regions inserted by Opari */
        POMP2_Init_regions();
    }

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Adapter finalization function.
 */
void
scorep_pomp_adapter_finalize( void )
{
    size_t       i;
    const size_t nRegions = POMP2_Get_num_regions();

    UTILS_DEBUG_ENTRY();

    if ( scorep_pomp_is_finalized || !scorep_pomp_is_initialized )
    {
        return;
    }

    scorep_pomp_is_finalized  = true;
    scorep_pomp_is_tracing_on = false;

    SCOREP_MutexDestroy( &scorep_pomp_assign_lock );

    if ( scorep_pomp_regions )
    {
        for ( i = 0; i < nRegions; ++i )
        {
            scorep_pomp_free_region_members( &scorep_pomp_regions[ i ] );
        }
        free( scorep_pomp_regions );
        scorep_pomp_regions = 0;
    }
    while ( scorep_region_list_top != NULL )
    {
        scorep_stack_node* next = scorep_region_list_top->next;
        free( scorep_region_list_top );
        scorep_region_list_top = next;
    }
}

/* ***********************************************************************
 *                                           C pomp function library
 ************************************************************************/


void
POMP2_Assign_handle( POMP2_Region_handle* pomp_handle,
                     const char           init_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Assign_handle" );

    /* Index counter */
    static size_t count = 0;

    /* If we have on-the-fly registration, we might need to increase the
       buffer for our regions */
    SCOREP_Pomp_Region* new_handle = ( count < POMP2_Get_num_regions() ?
                                       &scorep_pomp_regions[ count ] :
                                       scorep_pomp_allocate_region_on_the_fly() );

    /* Initialize new region struct */
    SCOREP_Pomp_ParseInitString( init_string, new_handle );

    /* Set return value */
    *pomp_handle = new_handle;

    /* Increase array index */
    ++count;
}
