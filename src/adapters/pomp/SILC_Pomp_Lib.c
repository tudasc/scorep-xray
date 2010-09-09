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
 * @ingroup    POMP
 *
 * @brief Implementation of the POMP OpenMP adapter functions.
 */

#include <config.h>
#include "SILC_Events.h"
#include "SILC_RuntimeManagement.h"
#include "pomp_lib.h"
#include "SILC_Pomp_RegionInfo.h"
#include "silc_utility/SILC_Utils.h"
#include "SILC_Pomp_Variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SILC_Pomp_Lock.h"

/** @ingroup POMP
    @{
 */

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP_Atomic_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Atomic_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Atomic_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Atomic_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Barrier_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Barrier_enter" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Barrier_exit" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Flush_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Flush_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Flush_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Critical_begin( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Critical_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Critical_end( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Critical_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Critical_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Critical_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Critical_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Critical_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_For_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_For_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_For_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_For_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Master_begin( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Master_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Master_end( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Master_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Parallel_begin( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Parallel_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerParallel );
    }
}

void
POMP_Parallel_end( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Parallel_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerParallel );
    }
}

void
POMP_Parallel_fork( POMP_Region_handle pomp_handle,
                    uint32_t           num_threads )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Parallel_fork" );
    /* If adapter is not initialized, initialize measurement */
    if ( !silc_pomp_is_initialized )
    {
        SILC_InitMeasurement();
    }

    /* Generate fork event */
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ASSERT( region != NULL );
        SILC_OmpFork( region->outerParallel, num_threads );
    }
}

void
POMP_Parallel_join( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Parallel_join" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_OmpJoin( region->outerParallel );
    }
}

void
POMP_Section_begin( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Section_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Section_end( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Section_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Sections_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Sections_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Sections_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Sections_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Single_begin( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Single_begin" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->innerBlock );
    }
}

void
POMP_Single_end( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Single_end" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->innerBlock );
    }
}

void
POMP_Single_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Single_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Single_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Single_exit" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_ExitRegion( region->outerBlock );
    }
}

void
POMP_Workshare_enter( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Workshare_enter" );
    if ( silc_pomp_is_tracing_on )
    {
        SILC_Pomp_Region* region = ( SILC_Pomp_Region* )pomp_handle;
        SILC_EnterRegion( region->outerBlock );
    }
}

void
POMP_Workshare_exit( POMP_Region_handle pomp_handle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Workshare_exit" );
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
POMP_Init_lock( omp_lock_t* s )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Init_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Destroy_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Set_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Unset_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Test_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Init_nest_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Destroy_nest_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Set_nest_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Unset_nest_lock" );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_OPENMP, "In POMP_Test_nest_lock" );
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
