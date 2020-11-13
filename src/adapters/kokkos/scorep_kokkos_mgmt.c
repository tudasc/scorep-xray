/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Technische Universitaet Dresden, Germany
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
 *  Kokkos adapter.
 */

#include <config.h>

#include "scorep_kokkos.h"

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Paradigms.h>

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

#include <inttypes.h>

#include "scorep_kokkos_confvars.inc.c"

/**
 * Registers the required configuration variables of the Kokkos adapter to the
 * measurement system.
 *
 * @param subsystemId   ID of the subsystem
 *
 * @return SCOREP_SUCCESS on success, otherwise a error code is returned
 */
static SCOREP_ErrorCode
kokkos_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegister( "kokkos", scorep_kokkos_confvars );
}

/**
 * Initializes the Kokkos subsystem.
 *
 * @return SCOREP_SUCCESS on success, otherwise a error code is returned
 */
static SCOREP_ErrorCode
kokkos_subsystem_init( void )
{
    UTILS_DEBUG( "Kokkos init" );
    UTILS_DEBUG( "Selected options: %" PRIu64, scorep_kokkos_features );

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_KOKKOS,
        SCOREP_PARADIGM_CLASS_ACCELERATOR,
        "KOKKOS",
        SCOREP_PARADIGM_FLAG_NONE );

    return SCOREP_SUCCESS;
}

static void
kokkos_subsystem_finalize( void )
{
    UTILS_DEBUG( "Kokkos finialize" );
}

const SCOREP_Subsystem SCOREP_Subsystem_KokkosAdapter =
{
    .subsystem_name     = "Kokkos",
    .subsystem_register = &kokkos_subsystem_register,
    .subsystem_end      = NULL,
    .subsystem_init     = &kokkos_subsystem_init,
    .subsystem_finalize = &kokkos_subsystem_finalize
};
