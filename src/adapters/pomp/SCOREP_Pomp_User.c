/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
#include <SCOREP_Thread_Types.h>
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Events.h"
#include "SCOREP_Pomp_Init.h"
#include <opari2/pomp2_lib.h>
#include "SCOREP_Pomp_RegionInfo.h"
#include "SCOREP_Pomp_Lock.h"
#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Pomp_Variables.h"

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/** Pointer to an array of all pomp regions */
SCOREP_Pomp_Region* scorep_pomp_regions;

/** Contains the region handle for all implicit barriers */
SCOREP_RegionHandle scorep_pomp_implicit_barrier_region = SCOREP_INVALID_REGION;

/** Flag to indicate, wether POMP2 traceing is enable/disabled */
bool scorep_pomp_is_tracing_on = true;

/** Source file handle for pomp wrapper functions and implicit barrier region. */
SCOREP_SourceFileHandle scorep_pomp_file_handle = SCOREP_INVALID_SOURCE_FILE;

/** Flag to indicate wether the adapter is initialized */
bool scorep_pomp_is_initialized = false;

/** Flag to indicate wether the adapter is finalized */
bool scorep_pomp_is_finalized = false;

/* **************************************************************************************
                                                                       Internal functions
****************************************************************************************/

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
    SCOREP_ASSERT( source );
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
static SCOREP_Error_Code
scorep_pomp_register( size_t subsystem_id )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_register\n" );

    scorep_pomp_subsystem_id = subsystem_id;

    return SCOREP_SUCCESS;
}

/** Adapter initialization function.
 */
static SCOREP_Error_Code
scorep_pomp_init()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_init\n" );

    /* Initialize the adapter */
    if ( !scorep_pomp_is_initialized )
    {
        /* Set flag */
        scorep_pomp_is_initialized = true;

        /* If initialized from user instrumentation initialize measurement before. */
        SCOREP_InitMeasurement();

        /* Initialize file handle for OpenMP API functions */
        scorep_pomp_file_handle = SCOREP_DefineSourceFile( "OMP" );

        /* Allocate memory for your POMP2_Get_num_regions() regions */
        scorep_pomp_regions = calloc( POMP2_Get_num_regions(),
                                      sizeof( SCOREP_Pomp_Region ) );

        /* Initialize implicit barrier region */
#ifdef _OPENMP
        scorep_pomp_implicit_barrier_region =
            SCOREP_DefineRegion( "!$omp implicit barrier",
                                 scorep_pomp_file_handle,
                                 SCOREP_INVALID_LINE_NO,
                                 SCOREP_INVALID_LINE_NO,
                                 SCOREP_ADAPTER_POMP,
                                 SCOREP_REGION_OMP_IMPLICIT_BARRIER );

        /* Register regions for locking functions */
        scorep_pomp_register_lock_regions();
#endif  // _OPENMP

        /* Register regions inserted by Opari */
        POMP2_Init_regions();
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_EXIT,
                         "scorep_pomp_init done.\n" );

    return SCOREP_SUCCESS;
}

/** Allows initialization of location specific data. Nothing done inside this funcion. */
static SCOREP_Error_Code
scorep_pomp_init_location()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_init_location\n" );
    return SCOREP_SUCCESS;
}

/** Allows finaltialization of location specific data. Nothing done inside this funcion.
 */
static void
scorep_pomp_final_location( SCOREP_Location* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_final_location\n" );
}

/** Adapter finalialization function.
 */
static void
scorep_pomp_final()
{
    size_t       i;
    const size_t nRegions = POMP2_Get_num_regions();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_final\n" );

    if ( scorep_pomp_is_finalized || !scorep_pomp_is_initialized )
    {
        return;
    }

    scorep_pomp_is_finalized  = true;
    scorep_pomp_is_tracing_on = false;

    if ( scorep_pomp_regions )
    {
        for ( i = 0; i < nRegions; ++i )
        {
            scorep_pomp_free_region_members( &scorep_pomp_regions[ i ] );
        }
        free( scorep_pomp_regions );
        scorep_pomp_regions = 0;
    }
}

/** Called when the adapter is derigistered. Nothing done inside the function
 */
static void
scorep_pomp_deregister()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "In scorep_pomp_deregister\n" );
}

/** Struct which contains the adapter iniitialization and finalization functions for the
    POMP2 adapter.
 */
const SCOREP_Subsystem SCOREP_Pomp_Adapter =
{
    "POMP2 Adapter / Version 1.0",
    &scorep_pomp_register,
    &scorep_pomp_init,
    &scorep_pomp_init_location,
    &scorep_pomp_final_location,
    &scorep_pomp_final,
    &scorep_pomp_deregister
};

/* **************************************************************************************
 *                                                                C pomp function library
 ***************************************************************************************/



void
POMP2_Finalize()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "Enter POMP2_Finalize\n" );
}

void
POMP2_Init()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP | SCOREP_DEBUG_FUNCTION_ENTRY,
                         "Enter POMP2_Init\n" );

    /* If adapter is not initialized, it means that the measurement system is not
       initialized. */
    SCOREP_InitMeasurement();
}

void
POMP2_Off()
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = false;
}

void
POMP2_On()
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = true;
}

void
POMP2_Assign_handle( POMP2_Region_handle* pomp_handle,
                     const char           init_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Assign_handle" );
    /* Index counter */
    static size_t count = 0;

    /* Initialize new region struct */
    SCOREP_Pomp_ParseInitString( init_string, &scorep_pomp_regions[ count ] );

    /* Set return value */
    *pomp_handle = &scorep_pomp_regions[ count ];

    /* Increase array index */
    ++count;
    SCOREP_ASSERT( count <= POMP2_Get_num_regions() );
}

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( region->innerBlock != SCOREP_INVALID_REGION )
        {
            SCOREP_EnterRegion( region->innerBlock );
        }
    }
}

void
POMP2_End( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_End" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( region->innerBlock != SCOREP_INVALID_REGION )
        {
            SCOREP_ExitRegion( region->innerBlock );
        }
    }
}

/** @} */
