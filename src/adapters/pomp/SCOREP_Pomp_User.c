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
 * @file       SCOREP_Pomp_User.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 user adapter functions and initialization.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCOREP_Location.h>
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Events.h"
#include "SCOREP_Pomp_Init.h"
#include <opari2/pomp2_lib.h>
#include "SCOREP_Pomp_RegionInfo.h"
#include "SCOREP_Pomp_Lock.h"
#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include "SCOREP_Pomp_Variables.h"
#include <SCOREP_Memory.h>

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/** Pointer to an array of all pomp regions */
SCOREP_Pomp_Region* scorep_pomp_regions;

/** Flag to indicate, wether POMP2 traceing is enable/disabled */
bool scorep_pomp_is_tracing_on = true;

/** Source file handle for pomp wrapper functions and implicit barrier region. */
SCOREP_SourceFileHandle scorep_pomp_file_handle = SCOREP_INVALID_SOURCE_FILE;

/** Flag to indicate wether the adapter is initialized */
bool scorep_pomp_is_initialized = false;

/** Flag to indicate wether the adapter is finalized */
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

/* **************************************************************************************
                                                                       Internal functions
****************************************************************************************/

static SCOREP_Pomp_Region*
scorep_pomp_allocate_region_on_the_fly()
{
    scorep_stack_node* node = malloc( sizeof( scorep_stack_node ) );
    node->next             = scorep_region_list_top;
    scorep_region_list_top = node;
    return &node->region;
}

/** Frees allocated memory and sets the pointer to 0. Used to free members of a
    SCOREP_Pomp_Region instance.
    @param member Pointer to the pointer which should be freed. If member is 0 nothing
                  happens.
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

/** Frees allocated memory of all members of a SCOREP_Pomp_Region instance.
    @param region The region for which all members should be freed. If region is 0,
                  nothing happens.
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

/* **************************************************************************************
                                                                           Initialization
****************************************************************************************/

static size_t scorep_pomp_subsystem_id;

/** Adapter initialization function to allow registering configuration variables. No
    variables are regstered.
 */
static SCOREP_ErrorCode
scorep_pomp_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_subsystem_id = subsystem_id;

    return SCOREP_SUCCESS;
}

/** Adapter initialization function.
 */
static SCOREP_ErrorCode
scorep_pomp_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Initialize the adapter */
    if ( !scorep_pomp_is_initialized )
    {
        /* Set flag */
        scorep_pomp_is_initialized = true;

        /* If initialized from user instrumentation initialize measurement before. */
        SCOREP_InitMeasurement();

        /* Initialize file handle for OpenMP API functions */
        scorep_pomp_file_handle = SCOREP_Definitions_NewSourceFile( "OMP" );

        /* Allocate memory for your POMP2_Get_num_regions() regions */
        scorep_pomp_regions = calloc( POMP2_Get_num_regions(),
                                      sizeof( SCOREP_Pomp_Region ) );

#ifdef _OPENMP
        /* Register regions for locking functions */
        scorep_pomp_register_lock_regions();
#endif  // _OPENMP

        SCOREP_MutexCreate( &scorep_pomp_assign_lock );

        /* Register regions inserted by Opari */
        POMP2_Init_regions();
    }

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Allows initialization of location specific data. Nothing done inside this funcion. */
static SCOREP_ErrorCode
scorep_pomp_init_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
    return SCOREP_SUCCESS;
}

/** Allows finaltialization of location specific data. Nothing done inside this funcion.
 */
static void
scorep_pomp_finalize_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
}

/** Adapter finalialization function.
 */
static void
scorep_pomp_finalize( void )
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

/** Called when the adapter is derigistered. Nothing done inside the function
 */
static void
scorep_pomp_deregister( void )
{
    UTILS_DEBUG_ENTRY();
}

/** Struct which contains the adapter iniitialization and finalization functions for the
    POMP2 adapter.
 */
const SCOREP_Subsystem SCOREP_Pomp_Adapter =
{
    .subsystem_name              = "POMP2 Adapter / Version 1.0",
    .subsystem_register          = &scorep_pomp_register,
    .subsystem_init              = &scorep_pomp_init,
    .subsystem_init_location     = &scorep_pomp_init_location,
    .subsystem_finalize_location = &scorep_pomp_finalize_location,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &scorep_pomp_finalize,
    .subsystem_deregister        = &scorep_pomp_deregister
};

/* **************************************************************************************
 *                                                                C pomp function library
 ***************************************************************************************/



void
POMP2_Finalize( void )
{
    UTILS_DEBUG_ENTRY();
}

void
POMP2_Init( void )
{
    UTILS_DEBUG_ENTRY();

    /* If adapter is not initialized, it means that the measurement system is not
       initialized. */
    SCOREP_InitMeasurement();
}

void
POMP2_Off( void )
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = false;
}

void
POMP2_On( void )
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = true;
}

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

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Begin( POMP2_Region_handle* pomp_handle,
             const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_End( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_End" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

/** @} */
