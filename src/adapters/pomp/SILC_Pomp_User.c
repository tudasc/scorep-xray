/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Pomp_User.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Implementation of the POMP user adapter functions and initialization.
 */

#include "SILC_RuntimeManagement.h"
#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_Pomp_Init.h"
#include "pomp_lib.h"
#include "SILC_Pomp_RegionInfo.h"
#include "SILC_Pomp_Lock.h"
#include "SILC_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @ingroup POMP
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/** Pointer to an array of all pomp regions */
SILC_Pomp_Region* silc_pomp_regions;

/** Contains the region handle for all implicit barriers */
SILC_RegionHandle silc_pomp_implicit_barrier_region = SILC_INVALID_REGION;

/** Flag to indicate, wether POMP traceing is enable/disabled */
bool silc_pomp_is_tracing_on = true;

/** Source file handle for pomp wrapper functions and implicit barrier region. */
SILC_SourceFileHandle silc_pomp_file_handle = SILC_INVALID_SOURCE_FILE;

/** Flag to indicate wether the adapter is initialized */
bool silc_pomp_is_initialized = false;

/* **************************************************************************************
                                                                       Internal functions
****************************************************************************************/

/** Frees allocated memory and sets the pointer to 0. Used to free members of a
    SILC_Pomp_Region instance.
    @param member Pointer to the pointer which should be freed. If member is 0 nothing
                  happens.
 */
static void
silc_pomp_free_region_member( char** member )
{
    if ( *member )
    {
        free( *member );
        *member = 0;
    }
}

/** Frees allocated memory of all members of a SILC_Pomp_Region instance.
    @param region The region for which all members should be freed. If region is 0,
                  nothing happens.
 */
static void
silc_pomp_free_region_members( SILC_Pomp_Region* region )
{
    if ( region )
    {
        silc_pomp_free_region_member( &region->name );
        silc_pomp_free_region_member( &region->startFileName );
        silc_pomp_free_region_member( &region->endFileName );
        silc_pomp_free_region_member( &region->regionName );
    }
}

/** Copys a string to a given position. Allocates the necessary space for the
    destination.
    @param destination Pointer to the string where the string is copied into.
    @param source      String which is copied.
 */
static void
silc_pomp_assign_string( char**      destination,
                         const char* source )
{
    SILC_ASSERT( source );
    *destination = malloc( strlen( source ) * sizeof( char ) + 1 );
    strcpy( *destination, source );
}

/* **************************************************************************************
                                                                           Initialization
****************************************************************************************/

/** Adapter initialization function to allow registering configuration variables. No
    variables are regstered.
 */
SILC_Error_Code
silc_pomp_register()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_register\n" );
    return SILC_SUCCESS;
}

/** Adapter initialization function.
 */
SILC_Error_Code
silc_pomp_init()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_init\n" );

    /* Initialize the adapter */
    if ( !silc_pomp_is_initialized )
    {
        /* Set flag */
        silc_pomp_is_initialized = true;

        /* If initialized from user instrumentation initialize measurement before. */
        SILC_InitMeasurement();

        /* Initialize file handle for implicit barrier */
        silc_pomp_file_handle = SILC_DefineSourceFile( "POMP" );

        /* Allocate memory for your POMP_Get_num_regions() regions */
        silc_pomp_regions = calloc( POMP_Get_num_regions(),
                                    sizeof( SILC_Pomp_Region ) );

        /* Initialize implicit barrier region */
#ifdef _OPENMP
        silc_pomp_implicit_barrier_region =
            SILC_DefineRegion( "implicit barrier",
                               silc_pomp_file_handle,
                               SILC_INVALID_LINE_NO,
                               SILC_INVALID_LINE_NO,
                               SILC_ADAPTER_POMP,
                               SILC_REGION_OMP_IMPLICIT_BARRIER );

        /* Register regions for locking functions */
        silc_pomp_register_lock_regions();
#endif  // _OPENMP

        /* Register regions inserted by Opari */
        POMP_Init_regions();
    }

    return SILC_SUCCESS;
}

/** Allows initialization of location specific data. Nothing done inside this funcion. */
SILC_Error_Code
silc_pomp_init_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_init_location\n" );
    return SILC_SUCCESS;
}

/** Allows finaltialization of location specific data. Nothing done inside this funcion.
 */
void
silc_pomp_final_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_final_location\n" );
}

/** Adapter finalialization function.
 */
void
silc_pomp_final()
{
    static int   pomp_finalize_called = 0;
    size_t       i;
    const size_t nRegions = POMP_Get_num_regions();

    if ( silc_pomp_regions )
    {
        for ( i = 0; i < nRegions; ++i )
        {
            silc_pomp_free_region_members( &silc_pomp_regions[ i ] );
        }
        free( silc_pomp_regions );
        silc_pomp_regions = 0;
    }

    if ( !pomp_finalize_called )
    {
        pomp_finalize_called = 1;
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_final\n" );
}

/** Called when the adapter is derigistered. Nothing done inside the function
 */
void
silc_pomp_deregister()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_pomp_deregister\n" );
}

/** Struct which contains the adapter iniitialization and finalization functions for the
    POMP adapter.
 */
struct SILC_Adapter SILC_Pomp_Adapter =
{
    SILC_ADAPTER_POMP,
    "POMP Adapter / Version 1.0",
    &silc_pomp_register,
    &silc_pomp_init,
    &silc_pomp_init_location,
    &silc_pomp_final_location,
    &silc_pomp_final,
    &silc_pomp_deregister
};

/* **************************************************************************************
 *                                                                C pomp function library
 ***************************************************************************************/



void
POMP_Finalize()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "Enter POMP_Finalize\n" );
}

void
POMP_Init()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP | SILC_DEBUG_FUNCTION_ENTRY,
                       "Enter POMP_Init\n" );

    /* If adapter is not initialized, it means that the measurement system is not
       initialized. */
    SILC_InitMeasurement();
}

void
POMP_Off()
{
    silc_pomp_is_tracing_on = false;
}

void
POMP_On()
{
    silc_pomp_is_tracing_on = true;
}

void
POMP_Assign_handle( POMP_Region_handle* pomp_handle,
                    const char          init_string[] )
{
    /* Index counter */
    static size_t count = 0;

    /* Initialize new region struct */
    SILC_Pomp_ParseInitString( init_string, &silc_pomp_regions[ count ] );

    /* Set return value */
    *pomp_handle = &silc_pomp_regions[ count ];

    /* Increase array index */
    ++count;
    SILC_ASSERT( count <= POMP_Get_num_regions() );
}

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP_Begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_End( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

/** @} */
