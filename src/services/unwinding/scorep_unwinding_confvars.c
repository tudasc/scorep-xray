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

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>

#include "scorep_unwinding_confvars.inc.c"

/** Our subsystem id, used to address our per-location unwinding data */
size_t scorep_unwinding_subsystem_id;

/**
 * Implementation of the adapter_register function of the
 * @ref SCOREP_Subsystem struct for the initialization
 * process of the unwinding adapter.
 */
static SCOREP_ErrorCode
unwinding_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_unwinding_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegisterCond(
        "unwinding",
        scorep_unwinding_confvars,
        HAVE_BACKEND_UNWINDING_SUPPORT );
}

/** The initialization struct for the unwinding adapter */
const SCOREP_Subsystem SCOREP_Subsystem_UnwindingService =
{
    .subsystem_name     = "Unwinding (config variables only)",
    .subsystem_register = &unwinding_subsystem_register,
};
