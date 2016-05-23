/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include "scorep_openacc_confvars.inc.c"

#define SCOREP_DEBUG_MODULE_NAME OPENACC
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>

/**
 * Registers the required configuration variables of the OpenACC adapter
 * to the measurement system.
 */
static SCOREP_ErrorCode
scorep_openacc_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegisterCond( "openacc",
                                      scorep_openacc_configs,
                                      HAVE_BACKEND_OPENACC_SUPPORT );
}

SCOREP_Subsystem SCOREP_Subsystem_OpenaccAdapter =
{
    .subsystem_name          = "OpenACC (config variables only)",
    .subsystem_register      = &scorep_openacc_register,
    .subsystem_end           = NULL,
    .subsystem_init          = NULL,
    .subsystem_init_location = NULL,
    .subsystem_pre_unify     = NULL,
    .subsystem_post_unify    = NULL
};
