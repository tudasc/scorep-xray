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

#include <scorep_unify_helpers.h>
#include <scorep_status.h>

#define SCOREP_DEBUG_MODULE_NAME KOKKOS
#include <UTILS_Debug.h>

#include <stdbool.h>
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

static SCOREP_InterimCommunicatorHandle kokkos_interim_communicator_handle =
    SCOREP_INVALID_INTERIM_COMMUNICATOR;

static void
init_host_location( SCOREP_Location* location )
{
    scorep_kokkos_cpu_location_data* data = SCOREP_Memory_AllocForMisc( sizeof( *data ) );
    data->active_deep_copy = false;
    SCOREP_Location_SetSubsystemData( location, scorep_kokkos_subsystem_id, data );
}

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
    data->rma_win_rank    = ++kokkos_location_rank;
    data->rma_win_created = false;

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

SCOREP_RmaWindowHandle
scorep_kokkos_define_rma_win( void )
{
    kokkos_interim_communicator_handle =
        SCOREP_Definitions_NewInterimCommunicator(
            SCOREP_INVALID_INTERIM_COMMUNICATOR,
            SCOREP_PARADIGM_KOKKOS,
            0, NULL );
    return SCOREP_Definitions_NewRmaWindow( "KOKKOS_WINDOW",
                                            kokkos_interim_communicator_handle );
}

static size_t
create_comm_group( uint64_t** globalLocationIds )
{
    size_t count = 1;

    /* get the number of CUDA communication partners */
    if ( kokkos_device_location != NULL )
    {
        count++;
    }

    if ( count == 0 )
    {
        return count;
    }

    /* allocate the CUDA communication group array */
    *globalLocationIds          = SCOREP_Memory_AllocForMisc( count * sizeof( uint64_t ) );
    ( *globalLocationIds )[ 0 ] = SCOREP_Location_GetGlobalId( SCOREP_Location_GetCurrentCPULocation() );
    if ( count > 1 )
    {
        ( *globalLocationIds )[ 1 ] = SCOREP_Location_GetGlobalId( kokkos_device_location );
    }

    return count;
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
        SCOREP_PARADIGM_FLAG_RMA_ONLY );

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
    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );
    if ( SCOREP_LOCATION_TYPE_CPU_THREAD == location_type )
    {
        init_host_location( location );
    }
    else if ( SCOREP_LOCATION_TYPE_GPU == location_type )
    {
        create_device_location( location );
    }
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
kokkos_subsystem_pre_unify( void )
{
    if ( kokkos_device_location == NULL )
    {
        return SCOREP_SUCCESS;
    }

    uint64_t* global_location_ids;
    size_t    global_location_number = create_comm_group( &global_location_ids );

    size_t   i      = 0;
    uint32_t offset = scorep_unify_helper_define_comm_locations(
        SCOREP_GROUP_KOKKOS_LOCATIONS, "KOKKOS",
        global_location_number, global_location_ids );
    UTILS_DEBUG( "Unifying %d location", global_location_number );

    /* add the offset */
    for ( i = 0; i < global_location_number; i++ )
    {
        global_location_ids[ i ] = i + offset;
    }

    SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_KOKKOS_GROUP, "KOKKOS_GROUP",
        global_location_number, global_location_ids );

    if ( kokkos_interim_communicator_handle !=
         SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        SCOREP_LOCAL_HANDLE_DEREF( kokkos_interim_communicator_handle,
                                   InterimCommunicator )->unified =
            SCOREP_Definitions_NewCommunicator(
                group_handle,
                SCOREP_INVALID_STRING,
                SCOREP_INVALID_COMMUNICATOR, 0 );
    }
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
kokkos_subsystem_post_unify( void )
{
    if ( kokkos_device_location == NULL )
    {
        return SCOREP_SUCCESS;
    }

    if ( 0 == SCOREP_Status_GetRank() )
    {
        /* Count the number of KOKKOS locations */
        uint32_t total_number_of_kokkos_locations = 0;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             Location,
                                                             location )
        {
            total_number_of_kokkos_locations++;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

        if ( total_number_of_kokkos_locations )
        {
            /* collect the global location ids for the KOKKOS locations */
            uint64_t kokkos_locations[ total_number_of_kokkos_locations ];
            total_number_of_kokkos_locations = 0;
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                                 Location,
                                                                 location )
            {
                kokkos_locations[ total_number_of_kokkos_locations++ ]
                    = definition->global_location_id;
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

            /* define the group of locations for all KOKKOS locations */
            SCOREP_Definitions_NewUnifiedGroup( SCOREP_GROUP_LOCATIONS,
                                                "KOKKOS_GROUP",
                                                total_number_of_kokkos_locations,
                                                kokkos_locations );
        }
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
    .subsystem_init_location = &kokkos_subsystem_init_location,
    .subsystem_pre_unify     = &kokkos_subsystem_pre_unify,
    .subsystem_post_unify    = &kokkos_subsystem_post_unify
};
