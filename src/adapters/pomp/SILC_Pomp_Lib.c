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
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 OpenMP adapter functions.
 */

#include <config.h>
#include <SILC_Events.h>
#include <SILC_RuntimeManagement.h>
#include <pomp2_lib.h>
#include "SILC_Pomp_RegionInfo.h"
#include <silc_utility/SILC_Utils.h>
#include "SILC_Pomp_Variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SILC_Pomp_Lock.h"

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Atomic_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Atomic_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Atomic_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Atomic_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Barrier_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Barrier_enter" );
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
POMP2_Barrier_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Barrier_exit" );
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
POMP2_Flush_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Flush_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Flush_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Flush_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Critical_begin( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Critical_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Critical_end( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Critical_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Critical_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Critical_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Critical_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Critical_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_For_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_For_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_For_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_For_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Master_begin( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Master_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Master_end( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Master_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Parallel_begin( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Parallel_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerParallel );
    }
}

void
POMP2_Parallel_end( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Parallel_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerParallel );
    }
}

void
POMP2_Parallel_fork( POMP2_Region_handle pomp_handle,
                     int                 num_threads )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Parallel_fork" );
    SILC_ASSERT( silc_pomp_is_initialized );

    /* Generate fork event */
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ASSERT( region != NULL );
        SILC_OmpFork( region->outerParallel, num_threads );
    }
}

void
POMP2_Parallel_join( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Parallel_join" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_OmpJoin( region->outerParallel );
    }
}

void
POMP2_Section_begin( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Section_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Section_end( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Section_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Sections_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Sections_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Sections_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Sections_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Single_begin( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Single_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Single_end( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Single_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Single_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Single_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Single_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Single_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Workshare_enter( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Workshare_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Workshare_exit( POMP2_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Workshare_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

/* **************************************************************************************
 *                                                                  C wrapper for OMP API
 ***************************************************************************************/

#ifdef _OPENMP

void
POMP2_Init_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Init_lock" );
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
POMP2_Destroy_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Destroy_lock" );
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
POMP2_Set_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Set_lock" );
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
POMP2_Unset_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Unset_lock" );
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
POMP2_Test_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Test_lock" );
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
POMP2_Init_nest_lock( omp_nest_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Init_nest_lock" );
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
POMP2_Destroy_nest_lock( omp_nest_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Destroy_nest_lock" );
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
POMP2_Set_nest_lock( omp_nest_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Set_nest_lock" );
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
POMP2_Unset_nest_lock( omp_nest_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Unset_nest_lock" );
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
POMP2_Test_nest_lock( omp_nest_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP2_Test_nest_lock" );
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

#endif // _OPENMP

/** @} */
