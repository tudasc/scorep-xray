/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
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

#define SCOREP_DEBUG_MODULE_NAME OPENCL
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include "scorep_opencl_config.h"

#include "scorep_opencl_confvars.inc.c"

/**
 * Registers the required configuration variables of the OpenCL adapter
 * to the measurement system.
 *
 * @param subsystemId       Subsystem identifier
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegisterCond( "opencl",
                                      scorep_opencl_confvars,
                                      HAVE_BACKEND_OPENCL_SUPPORT );
}

const SCOREP_Subsystem SCOREP_Subsystem_OpenclAdapter =
{
    .subsystem_name     = "OpenCL (config variables only)",
    .subsystem_register = &opencl_subsystem_register,
};
