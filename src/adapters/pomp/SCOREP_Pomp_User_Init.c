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
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 user adapter initialization.
 */

#include <config.h>

#include "SCOREP_Pomp_Common.h"

#include <SCOREP_Subsystem.h>

#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Debug.h>


/* *****************************************************************************
 *                                                         POMP User subsystem *
 ******************************************************************************/

static size_t scorep_pomp_user_subsystem_id;

/** Adapter initialization function to allow registering configuration
    variables. No variables are registered.
 */
static SCOREP_ErrorCode
pomp_user_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_user_subsystem_id = subsystem_id;

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Adapter initialization function.
 */
static SCOREP_ErrorCode
pomp_user_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Initialize the common POMP adapter */
    scorep_pomp_adapter_init();

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Adapter finalization function.
 */
static void
pomp_user_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_adapter_finalize();

    UTILS_DEBUG_EXIT();
}

/** Struct which contains the adapter initialization and finalization
    functions for the POMP2 user adapter.
 */
const SCOREP_Subsystem SCOREP_Subsystem_PompUserAdapter =
{
    .subsystem_name              = "POMP2 User Adapter / Version 1.0",
    .subsystem_register          = &pomp_user_subsystem_register,
    .subsystem_init              = &pomp_user_subsystem_init,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &pomp_user_subsystem_finalize,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
