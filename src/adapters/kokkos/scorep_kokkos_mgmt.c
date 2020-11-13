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
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

#include <inttypes.h>

#include "scorep_kokkos_confvars.inc.c"

size_t scorep_kokkos_subsystem_id;

/*
 * This gets a bit ugly for the moment.
 * Kokkosp doesn't provide multi-device support
 * or any device info at all, at present (3.1),
 * so we're left to fake it. General approach
 * is that host-side is local location 0, devices
 * [0...n] should be local locations [1...n+1].
 * The problem is that if and only if we lack
 * a "real" device can we create a fake Kokkos one
 * and record with host-side timestamps what's
 * occurring on the device side.
 */

/*
 * Location IDs for devices start from 1, as per
 * above
 */

/*
 * Kokkos guarantees that creating the device is happened sequentially,
 * thus no locking.
 */
static SCOREP_Location* kokkos_device_location;

static uint32_t kokkos_location_rank = 0;

static void
create_device_location( SCOREP_Location* location )
{
    static bool device_location_initialized = false;
    if ( device_location_initialized )
    {
        return;
    }
    device_location_initialized = true;
    UTILS_DEBUG( "Found device location" );

    scorep_kokkos_gpu_location_data* data = SCOREP_Memory_AllocForMisc( sizeof( *data ) );
    data->rma_win_rank = ++kokkos_location_rank;

    SCOREP_Location_SetSubsystemData( location, scorep_kokkos_subsystem_id, data );

    /* Only supporting one location at the moment. */
    if ( kokkos_device_location == NULL )
    {
        kokkos_device_location = location;
    }
}

SCOREP_Location*
scorep_kokkos_get_device_location( void )
{
    /*
     * Kokkos guarantees that there is no conucrrent running thread, which
     * also creates a PU location
     */
    if ( kokkos_device_location == NULL )
    {
        UTILS_DEBUG( "Creating artificial Kokkos device location" );
        SCOREP_Location* location = SCOREP_Location_CreateNonCPULocation(
            SCOREP_Location_GetCurrentCPULocation(),
            SCOREP_LOCATION_TYPE_GPU, "Kokkos Device" );
        /* create_device_location called through subsystem */
    }

    return kokkos_device_location;
}

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

    scorep_kokkos_subsystem_id = subsystemId;

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

static SCOREP_ErrorCode
kokkos_subsystem_init_location( SCOREP_Location* location,
                                SCOREP_Location* parent )
{
    if ( SCOREP_LOCATION_TYPE_GPU == SCOREP_Location_GetType( location ) )
    {
        create_device_location( location );
    }
    return SCOREP_SUCCESS;
}

const SCOREP_Subsystem SCOREP_Subsystem_KokkosAdapter =
{
    .subsystem_name          = "Kokkos",
    .subsystem_register      = &kokkos_subsystem_register,
    .subsystem_end           = NULL,
    .subsystem_init          = &kokkos_subsystem_init,
    .subsystem_finalize      = &kokkos_subsystem_finalize,
    .subsystem_init_location = &kokkos_subsystem_init_location
};
