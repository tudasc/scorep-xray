/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 *
 */

#include <config.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>

#define SCOREP_DEBUG_MODULE_NAME SAMPLING
#include <UTILS_Debug.h>

#include "scorep_sampling_confvars.inc.c"

/** Subsystem identifier */
static size_t scorep_sampling_subsystem_id;

/**
 * Implementation of the adapter_register function of the
 * @ref SCOREP_Subsystem struct for the initialization
 * process of the sampling adapter.
 */
static SCOREP_ErrorCode
scorep_sampling_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_sampling_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegisterCond( "sampling",
                                      scorep_sampling_confvars,
                                      HAVE_BACKEND_SAMPLING_SUPPORT );
}

/** The initialization struct for the sampling adapter */
const SCOREP_Subsystem SCOREP_Subsystem_SamplingService =
{
    .subsystem_name     = "Sampling (config variables only)",
    .subsystem_register = &scorep_sampling_register,
};
