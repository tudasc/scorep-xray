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
 * @file       SILC_Pomp_Lib.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Implementation of the POMP adapter functions.
 */

#if 0

#include "SILC_Pomp_Init.h"
#include "pomp_lib.h"
#include "SILC_Pomp_RegionInfo.h"
#include "SILC_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SILC_Pomp_Lock.h"

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
/** Flag to indicate wether the adapter is initialized */
bool silc_pomp_is_initialized = false;

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

        /* Register regions */
        silc_pomp_register_lock_regions();
        POMP_Init_regions();

        /* Initialize implicit barrier region */
        silc_pomp_implicit_barrier_region =
            SILC_DefineRegion( "implicit barrier",
                               silc_pomp_file_handle,
                               SILC_INVALID_LINE_NO,
                               SILC_INVALID_LINE_NO,
                               SILC_ADAPTER_POMP,
                               SILC_REGION_OMP_IMPLICIT_BARRIER );
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
}

void
POMP_Init()
{
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

void
POMP_Atomic_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Atomic_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Barrier_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        if ( region->regionType == SILC_Pomp_Barrier )
        {
            SILC_EnterRegion( region->outerBlock );
        }
        else
        {
            SILC_EnterRegion( silc_pomp_implicit_barrier_region );
        }
    }
}

void
POMP_Barrier_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        if ( region->regionType == SILC_Pomp_Barrier )
        {
            SILC_ExitRegion( region->outerBlock );
        }
        else
        {
            SILC_ExitRegion( silc_pomp_implicit_barrier_region );
        }
    }
}

void
POMP_Flush_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Flush_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Critical_begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Critical_end( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Critical_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Critical_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_For_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_For_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Master_begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Master_end( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Parallel_begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerParallel );
    }
}

void
POMP_Parallel_end( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerParallel );
    }
}

void
POMP_Parallel_fork( POMP_Region_handle pomp_handle )
{
    /* If adapter is not initialized, initialize measurement */
    if ( !silc_pomp_is_initialized )
    {
        SILC_InitMeasurement();
    }

    /* Generate fork event */
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_OmpFork( region->outerParallel );
    }
}

void
POMP_Parallel_join( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_OmpJoin( region->outerParallel );
    }
}

void
POMP_Section_begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Section_end( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Sections_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Sections_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Single_begin( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Single_end( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Single_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Single_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Workshare_enter( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Workshare_exit( POMP_Region_handle pomp_handle )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

/* **************************************************************************************
 *                                                                  C wrapper for OMP API
 ***************************************************************************************/

void
POMP_Init_lock( omp_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_INIT_LOCK ] );
        omp_init_lock( s );
        silc_pomp_lock_init( s );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_INIT_LOCK ] );
    }
    else
    {
        omp_init_lock( s );
        silc_pomp_lock_init( s );
    }
}

void
POMP_Destroy_lock( omp_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_DESTROY_LOCK ] );
        omp_destroy_lock( s );
        silc_pomp_lock_destroy( s );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_DESTROY_LOCK ] );
    }
    else
    {
        omp_destroy_lock( s );
        silc_pomp_lock_destroy( s );
    }
}

void
POMP_Set_lock( omp_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_SET_LOCK ] );
        omp_set_lock( s );
        SILC_OmpAcquireLock( silc_pomp_get_lock_handle( s ) );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_SET_LOCK ] );
    }
    else
    {
        omp_set_lock( s );
    }
}

void
POMP_Unset_lock( omp_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_UNSET_LOCK ] );
        omp_unset_lock( s );
        SILC_OmpReleaseLock( silc_pomp_get_lock_handle( s ) );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_UNSET_LOCK ] );
    }
    else
    {
        omp_unset_lock( s );
    }
}

int
POMP_Test_lock( omp_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        int result;

        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_TEST_LOCK ] );
        result = omp_test_lock( s );
        if ( result )
        {
            SILC_OmpAcquireLock( silc_pomp_get_lock_handle( s ) );
        }
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_TEST_LOCK ] );
        return result;
    }
    else
    {
        return omp_test_lock( s );
    }
}

void
POMP_Init_nest_lock( omp_nest_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_INIT_NEST_LOCK ] );
        omp_init_nest_lock( s );
        silc_pomp_lock_init( s );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_INIT_NEST_LOCK ] );
    }
    else
    {
        omp_init_nest_lock( s );
        silc_pomp_lock_init( s );
    }
}

void
POMP_Destroy_nest_lock( omp_nest_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_DESTROY_NEST_LOCK ] );
        omp_destroy_nest_lock( s );
        silc_pomp_lock_destroy( s );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_DESTROY_NEST_LOCK ] );
    }
    else
    {
        omp_destroy_nest_lock( s );
        silc_pomp_lock_destroy( s );
    }
}

void
POMP_Set_nest_lock( omp_nest_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_SET_NEST_LOCK ] );
        omp_set_nest_lock( s );
        SILC_OmpAcquireLock( silc_pomp_get_lock_handle( s ) );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_SET_NEST_LOCK ] );
    }
    else
    {
        omp_set_nest_lock( s );
    }
}

void
POMP_Unset_nest_lock( omp_nest_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_UNSET_NEST_LOCK ] );
        omp_unset_nest_lock( s );
        SILC_OmpReleaseLock( silc_pomp_get_lock_handle( s ) );
        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_UNSET_NEST_LOCK ] );
    }
    else
    {
        omp_unset_nest_lock( s );
    }
}

int
POMP_Test_nest_lock( omp_nest_lock_t* s )
{
    if ( silc_pomp_is_tracing_on )
    {
        int result;

        SILC_EnterRegion( silc_pomp_regid[ SILC_POMP_TEST_NEST_LOCK ] );
        result = omp_test_nest_lock( s );
        if ( result )
        {
            SILC_OmpAcquireLock( silc_pomp_get_lock_handle( s ) );
        }

        SILC_ExitRegion( silc_pomp_regid[ SILC_POMP_TEST_NEST_LOCK ] );
        return result;
    }
    else
    {
        return omp_test_nest_lock( s );
    }
}

/** @} */

#endif // if 0
