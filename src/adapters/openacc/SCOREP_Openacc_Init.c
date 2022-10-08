/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2016, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file contains the implementation of the initialization functions of the
 *  OpenACC adapter.
 */

#include <config.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Paradigms.h>

#define SCOREP_DEBUG_MODULE_NAME OPENACC
#include <UTILS_Debug.h>

#include "scorep_openacc.h"

#include "scorep_openacc_confvars.inc.c"

/* OpenACC mutex */
UTILS_Mutex scorep_openacc_mutex = UTILS_MUTEX_INIT;

/**
 * Registers the required configuration variables of the OpenACC adapter to the
 * measurement system.
 *
 * @param subsystemId   ID of the subsystem
 *
 * @return SCOREP_SUCCESS on success, otherwise a error code is returned
 */
static SCOREP_ErrorCode
openacc_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegister( "openacc", scorep_openacc_confvars );
}

/**
 * Initializes the OpenACC subsystem.
 *
 * @return SCOREP_SUCCESS on success, otherwise a error code is returned
 */
static SCOREP_ErrorCode
openacc_subsystem_init( void )
{
    UTILS_DEBUG( "Selected options: %llu", scorep_openacc_features );

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_OPENACC,
        SCOREP_PARADIGM_CLASS_ACCELERATOR,
        "OpenACC",
        SCOREP_PARADIGM_FLAG_RMA_ONLY );

    if ( scorep_openacc_features > 0 )
    {
        scorep_openacc_setup_features();
    }

    return SCOREP_SUCCESS;
}

const SCOREP_Subsystem SCOREP_Subsystem_OpenaccAdapter =
{
    .subsystem_name          = "OPENACC",
    .subsystem_register      = &openacc_subsystem_register,
    .subsystem_end           = NULL,
    .subsystem_init          = &openacc_subsystem_init,
    .subsystem_init_location = NULL,
    .subsystem_finalize      = NULL,
    .subsystem_pre_unify     = NULL,
    .subsystem_post_unify    = NULL
};
