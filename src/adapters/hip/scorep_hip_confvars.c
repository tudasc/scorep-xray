/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>

#include "scorep_hip_confvars.inc.c"

/**
 * Registers the required configuration variables of the HIP adapter
 * to the measurement system.
 */
static SCOREP_ErrorCode
scorep_hip_register( size_t subsystemId )
{
    return SCOREP_ConfigRegisterCond( "hip",
                                      scorep_hip_confvars,
                                      HAVE_BACKEND_HIP_SUPPORT );
}

const SCOREP_Subsystem SCOREP_Subsystem_HipAdapter =
{
    .subsystem_name     = "HIP (config variables only)",
    .subsystem_register = &scorep_hip_register,
};
