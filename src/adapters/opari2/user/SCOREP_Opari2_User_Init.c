/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Implementation of the OPARI2 user adapter initialization.
 */

#include <config.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>

#include <opari2/pomp2_user_lib.h>

#include "SCOREP_Opari2_User_Regions.h"

#define SCOREP_DEBUG_MODULE_NAME OPARI2
#include <UTILS_Debug.h>


SCOREP_Opari2_User_Region* scorep_opari2_user_regions;

/** Flag to indicate whether the adapter is initialized */
bool scorep_opari2_user_is_initialized = false;

/** Flag to indicate whether the adapter is finalized */
bool scorep_opari2_user_is_finalized = false;

/** Lock to protect on-the-fly assignments.*/
SCOREP_Mutex scorep_opari2_user_assign_lock;


/* *****************************************************************************
 *                                                        OPARI User subsystem *
 ******************************************************************************/

static size_t subsystem_id;

/** Adapter initialization function to allow registering configuration
    variables. No variables are registered.
 */
static SCOREP_ErrorCode
opari2_user_subsystem_register( size_t id )
{
    UTILS_DEBUG_ENTRY();

    subsystem_id = id;

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Adapter initialization function.
 */
static SCOREP_ErrorCode
opari2_user_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    if ( !scorep_opari2_user_is_initialized )
    {
        scorep_opari2_user_is_initialized = true;
        SCOREP_MutexCreate( &scorep_opari2_user_assign_lock );

        size_t n = POMP2_USER_Get_num_regions();

        scorep_opari2_user_regions = calloc( n, sizeof( SCOREP_Opari2_User_Region ) );

        /* Initialize regions inserted by Opari */
        POMP2_USER_Init_regions();

        SCOREP_SourceFileHandle scorep_opari2_user_file_handle
            = SCOREP_Definitions_NewSourceFile( "POMP" );
    }
    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}


static void
opari2_user_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    if ( !scorep_opari2_user_is_initialized ||
         scorep_opari2_user_is_finalized )
    {
        return;
    }
    scorep_opari2_user_is_finalized = true;

    free( scorep_opari2_user_regions );
    SCOREP_MutexDestroy( &scorep_opari2_user_assign_lock );

    UTILS_DEBUG_EXIT();
}

const SCOREP_Subsystem SCOREP_Subsystem_Opari2UserAdapter =
{
    .subsystem_name              = "OPARI2 Adapter for the POMP2 User interface / Version 1.0",
    .subsystem_register          = &opari2_user_subsystem_register,
    .subsystem_init              = &opari2_user_subsystem_init,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &opari2_user_subsystem_finalize,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
