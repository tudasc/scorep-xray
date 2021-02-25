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

#include <config.h>

#include "scorep_kokkos.h"

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>

#include "scorep_kokkos_confvars.inc.c"

/**
 * Registers the required configuration variables of the Kokkos adapter
 * to the measurement system.
 */
static SCOREP_ErrorCode
scorep_kokkos_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegisterCond( "kokkos",
                                      scorep_kokkos_confvars,
                                      HAVE_BACKEND_KOKKOS_SUPPORT );
}

const SCOREP_Subsystem SCOREP_Subsystem_KokkosAdapter =
{
    .subsystem_name     = "Kokkos",
    .subsystem_register = &scorep_kokkos_register,
};
