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
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 OpenMP adapter initialization.
 */

#include <config.h>

#include "SCOREP_Pomp_Common.h"
#include "SCOREP_Pomp_Lock.h"

#include <SCOREP_Subsystem.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Debug.h>


/* ***************************************************************************
*                                                         POMP lock regions *
*****************************************************************************/

/** List of registered omp function names. They must be in the same order as the
    corresponding SCOREP_Pomp_LockRegion_Index.
 */
static char* scorep_pomp_lock_region_names[] =
{
    "omp_init_lock",
    "omp_destroy_lock",
    "omp_set_lock",
    "omp_unset_lock",
    "omp_test_lock",
    "omp_init_nest_lock",
    "omp_destroy_nest_lock",
    "omp_set_nest_lock",
    "omp_unset_nest_lock",
    "omp_test_nest_lock"
};

/** List of handles for omp regions. The handles must be stored in the same order as
    the corresponding SCOREP_Pomp_LockRegion_Index.
 */
SCOREP_RegionHandle scorep_pomp_lock_region_handles[ SCOREP_POMP_REGION_LOCK_NUM ];


/* *****************************************************************************
 *                                                       POMP OpenMP subsystem *
 ******************************************************************************/

extern SCOREP_ErrorCode
scorep_omp_create_location_data( SCOREP_Location* location );
extern void
scorep_omp_destroy_location_data( SCOREP_Location* location );
extern void
scorep_omp_unify_thread_teams( void );
extern void
scorep_omp_unify_thread_teams_finalize( void );

size_t scorep_pomp_omp_subsystem_id;

static SCOREP_ErrorCode
scorep_pomp_omp_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_omp_subsystem_id = subsystem_id;

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
scorep_pomp_omp_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Initialize the common POMP adapter */
    scorep_pomp_adapter_init();

    SCOREP_SourceFileHandle scorep_pomp_file_handle
        = SCOREP_Definitions_NewSourceFile( "OMP" );

    int i = 0;
    for ( i = 0; i < SCOREP_POMP_REGION_LOCK_NUM; i++ )
    {
        scorep_pomp_lock_region_handles[ i ] =
            SCOREP_Definitions_NewRegion( scorep_pomp_lock_region_names[ i ],
                                          NULL,
                                          scorep_pomp_file_handle,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_OPENMP,
                                          SCOREP_REGION_WRAPPER );
    }

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
scorep_pomp_omp_init_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
    if ( SCOREP_LOCATION_TYPE_CPU_THREAD == SCOREP_Location_GetType( locationData ) )
    {
        return scorep_omp_create_location_data( locationData );
    }
    return SCOREP_SUCCESS;
}

static void
scorep_pomp_omp_finalize_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
    if ( SCOREP_LOCATION_TYPE_CPU_THREAD == SCOREP_Location_GetType( locationData ) )
    {
        scorep_omp_destroy_location_data( locationData );
    }
}

static SCOREP_ErrorCode
scorep_pomp_omp_pre_unify( void )
{
    scorep_omp_unify_thread_teams();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
scorep_pomp_omp_post_unify( void )
{
    scorep_omp_unify_thread_teams_finalize();

    return SCOREP_SUCCESS;
}

static void
scorep_pomp_omp_finalize( void )
{
    UTILS_DEBUG_ENTRY();
}

static void
scorep_pomp_omp_deregister( void )
{
    UTILS_DEBUG_ENTRY();
}

/** Struct which contains the adapter iniitialization and finalization
    functions for the POMP2 adapter.
 */
const SCOREP_Subsystem SCOREP_Subsystem_PompOmpAdapter =
{
    .subsystem_name              = "POMP2 OpenMP Adapter / Version 1.0",
    .subsystem_register          = &scorep_pomp_omp_register,
    .subsystem_init              = &scorep_pomp_omp_init,
    .subsystem_init_location     = &scorep_pomp_omp_init_location,
    .subsystem_finalize_location = &scorep_pomp_omp_finalize_location,
    .subsystem_pre_unify         = &scorep_pomp_omp_pre_unify,
    .subsystem_post_unify        = &scorep_pomp_omp_post_unify,
    .subsystem_finalize          = &scorep_pomp_omp_finalize,
    .subsystem_deregister        = &scorep_pomp_omp_deregister,
    .subsystem_control           = NULL
};
