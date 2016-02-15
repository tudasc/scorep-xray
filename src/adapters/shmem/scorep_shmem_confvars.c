/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
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
 *
 */

#include <config.h>

#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>

#define SCOREP_DEBUG_MODULE_NAME SHMEM
#include <UTILS_Debug.h>

#include "scorep_shmem_confvars.inc.c"

static size_t subsystem_id;

static SCOREP_ErrorCode
shmem_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();

    subsystem_id = subsystemId;

    return SCOREP_ConfigRegisterCond( "shmem",
                                      scorep_shmem_confvars,
                                      HAVE_BACKEND_SHMEM_SUPPORT );
}

/* The initialization struct for the SHMEM adapter */
const SCOREP_Subsystem SCOREP_Subsystem_ShmemAdapter =
{
    .subsystem_name     = "SHMEM (config variables only)",
    .subsystem_register = &shmem_subsystem_register
};
