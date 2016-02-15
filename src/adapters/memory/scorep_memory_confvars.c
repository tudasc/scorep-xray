/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
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

#define SCOREP_DEBUG_MODULE_NAME MEMORY
#include <UTILS_Debug.h>

#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>

#include "scorep_memory_mgmt.h"

#include "scorep_memory_confvars.inc.c"

static size_t memory_subsystem_id;

/** Registers the required configuration variables of the Memory adapter
    to the measurement system.
 */
static SCOREP_ErrorCode
memory_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG( "Register environment variables" );
    memory_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegisterCond( "memory",
                                      scorep_memory_confvars,
                                      HAVE_BACKEND_MEMORY_SUPPORT );
}


/** Struct which contains the adapter initialization and finalization functions for the
    MEMORY adapter.
 */
const SCOREP_Subsystem SCOREP_Subsystem_MemoryAdapter =
{
    .subsystem_name     = "MEMORY Adapter / Version 1.0 (config variables only)",
    .subsystem_register = &memory_subsystem_register
};
