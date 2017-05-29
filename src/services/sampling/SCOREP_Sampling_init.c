/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Config.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Subsystem.h>

#include <UTILS_Error.h>

#define SCOREP_DEBUG_MODULE_NAME SAMPLING
#include <UTILS_Debug.h>

#include <UTILS_CStr.h>

#include "SCOREP_Sampling.h"

#include <string.h>

#include "scorep_sampling_confvars.inc.c"

/** Lock for adapter initialization */
static SCOREP_Mutex init_sampling_mutex;
/** Subsystem identifier */
static size_t sampling_subsystem_id;
/** Sampling sources */
static scorep_sampling_interrupt_generator_definition* sampling_sources = NULL;
/** Number of available sampling sources */
static size_t num_sampling_sources = 0;


/* *********************************************************************
 * Implementation of the subsystem API
 * ********************************************************************/

/**
 * Register the required configuration variables of the sampling
 * adapter to the measurement system.
 *
 * @param subsystemId       Subsystem identifier
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
sampling_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();

    sampling_subsystem_id = subsystemId;
    SCOREP_ErrorCode status = SCOREP_ConfigRegister( "sampling",
                                                     scorep_sampling_confvars );

    return status;
}

/**
 * Initialize adapter and its global variables.
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
sampling_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Return if user did not enabled stack unwinding */
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    SCOREP_ErrorCode result = SCOREP_MutexCreate( &init_sampling_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "init_sampling_mutex could not be created" );

    /** Content of environment variable SCOREP_SAMPLING_EVENTS */
    char* env_sampling;
    /** Individual metric */
    char* token;

    /* Get working copy of the sampling source names. */
    env_sampling = UTILS_CStr_dup( scorep_sampling_events );

    /* Return if environment variable is empty */
    size_t str_len = strlen( env_sampling );
    if ( str_len == 0 )
    {
        goto out;
    }

    /* Count number of separator characters in list of sampling event names */
    size_t      list_alloc = 1;
    const char* position   = env_sampling;
    while ( *position )
    {
        if ( strchr( scorep_sampling_separator, *position ) )
        {
            list_alloc++;
        }
        position++;
    }

    /* Allocate memory for array of metric names */
    sampling_sources = calloc( list_alloc, sizeof( scorep_sampling_interrupt_generator_definition ) );
    if ( !sampling_sources )
    {
        UTILS_ERROR_POSIX();
        goto out;
    }

    /* Parse list of metric names */
    size_t list_len = 0;
    token = strtok( env_sampling, scorep_sampling_separator );
    while ( token )
    {
        if ( list_len >= list_alloc )
        {
            /* something strange has happened, we have
             * more entries as in the first run */
            free( sampling_sources );
            sampling_sources     = NULL;
            num_sampling_sources = 0;
            return SCOREP_ERROR_INVALID;
        }
        /* check */
        if ( strstr( token, "timer" ) == token )
        {
            sampling_sources[ list_len ].period = 1000;
            sampling_sources[ list_len ].type   = SCOREP_SAMPLING_TRIGGER_ITIMER;
        }
        else
        {
            if ( strstr( token, "perf" ) == token )
            {
                sampling_sources[ list_len ].period = 1000000;
                sampling_sources[ list_len ].type   = SCOREP_SAMPLING_TRIGGER_PERF;
            }
            else
            {
                sampling_sources[ list_len ].period = 1000000;
                sampling_sources[ list_len ].type   = SCOREP_SAMPLING_TRIGGER_PAPI;
            }
        }
        /* separate in eventname and period */
        char* divider = strrchr( token, '@' );
        if ( divider == NULL )
        {
            /* use default value */
            sampling_sources[ list_len ].event = strdup( token );
        }
        else
        {
            divider++;
            sampling_sources[ list_len ].period = atoll( divider );
            divider--;
            *divider                           = '\0';
            sampling_sources[ list_len ].event = strdup( token );
            *divider                           = '@';
        }

        token = strtok( NULL, scorep_sampling_separator );
        scorep_initialize_interrupt_sources( sampling_sources[ list_len ] );
        list_len++;
    }
    num_sampling_sources = list_len;

out:

    free( env_sampling );
    return SCOREP_SUCCESS;
}

/**
 * If @ location is a CPU thread the sampling adapter will be
 * initialized for this location.
 *
 * @param location          Current location.
 * @param parent            Location data from the creator.
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
sampling_subsystem_init_location( SCOREP_Location* location,
                                  SCOREP_Location* parent )
{
    UTILS_DEBUG_ENTRY();

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    if ( ( sampling_sources != NULL ) && SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        SCOREP_MutexLock( init_sampling_mutex );
        SCOREP_Sampling_LocationData* sampling_data =
            SCOREP_Location_AllocForMisc( location,
                                          sizeof( *sampling_data ) );
        sampling_data->data =
            SCOREP_Location_AllocForMisc( location,
                                          num_sampling_sources * sizeof( *sampling_data->data ) );

        SCOREP_Location_SetSubsystemData( location,
                                          sampling_subsystem_id,
                                          sampling_data );
        for ( size_t i = 0; i < num_sampling_sources; i++ )
        {
            memset( &( sampling_data->data[ i ] ), 0, sizeof( sampling_data->data[ i ] ) );
        }
        scorep_create_interrupt_sources( sampling_data, sampling_sources, num_sampling_sources );
        sampling_data->nr_data = num_sampling_sources;

        SCOREP_MutexUnlock( init_sampling_mutex );

        sig_atomic_t touch = scorep_sampling_is_known_pthread;
    }

    return SCOREP_SUCCESS;
}

/**
 * If @ location is a CPU thread the sampling adapter will be explicitly
 * enabled for this location.
 *
 * @param location          Current location.
 */
static SCOREP_ErrorCode
sampling_subsystem_activate_cpu_location( SCOREP_Location*        location,
                                          SCOREP_Location*        parentLocation,
                                          uint32_t                forksequenceCount,
                                          SCOREP_CPULocationPhase phase )
{
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    if ( sampling_sources == NULL )
    {
        return SCOREP_SUCCESS;
    }

    if ( phase == SCOREP_CPU_LOCATION_PHASE_MGMT )
    {
        return SCOREP_SUCCESS;
    }

    SCOREP_Sampling_LocationData* location_data =
        SCOREP_Location_GetSubsystemData( location, sampling_subsystem_id );
    scorep_enable_interrupt_sources( location_data, sampling_sources, num_sampling_sources );

    return SCOREP_SUCCESS;
}

/**
 * If @ location is a CPU thread the sampling adapter will be explicitly
 * disabled for this location.
 *
 * @param location          Current location.
 * @param phase             Current phase of the CPU locations (active or deactive).
 */
static void
sampling_subsystem_deactivate_cpu_location( SCOREP_Location*        location,
                                            SCOREP_Location*        parentLocation,
                                            SCOREP_CPULocationPhase phase )
{
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return;
    }

    if ( sampling_sources == NULL )
    {
        return;
    }

    if ( phase == SCOREP_CPU_LOCATION_PHASE_MGMT )
    {
        return;
    }

    SCOREP_Sampling_LocationData* location_data =
        SCOREP_Location_GetSubsystemData( location, sampling_subsystem_id );
    scorep_disable_interrupt_sources( location_data, sampling_sources, num_sampling_sources );
}

/**
 * Location specific finalization function. This function will free
 * all resources of the interrupt sources.
 *
 * @param location             Location data.
 */
static void
sampling_subsystem_finalize_location( SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return;
    }

    if ( sampling_sources == NULL )
    {
        return;
    }

    SCOREP_Sampling_LocationData* location_data =
        SCOREP_Location_GetSubsystemData( location, sampling_subsystem_id );

    if ( location_data == NULL )
    {
        return;
    }

    scorep_finalize_interrupt_sources( location_data, sampling_sources, num_sampling_sources );
}

/**
 * Finalization of the sampling adapter, free global variables.
 */
static void
sampling_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return;
    }

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &init_sampling_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "init_sampling_mutex could not be destroyed" );
}


/** Sampling adapter with its callbacks */
const SCOREP_Subsystem SCOREP_Subsystem_SamplingService =
{
    .subsystem_name                    = "Sampling Adapter / Version 1.0",
    .subsystem_register                = &sampling_subsystem_register,
    .subsystem_init                    = &sampling_subsystem_init,
    .subsystem_init_location           = &sampling_subsystem_init_location,
    .subsystem_activate_cpu_location   = &sampling_subsystem_activate_cpu_location,
    .subsystem_deactivate_cpu_location = &sampling_subsystem_deactivate_cpu_location,
    .subsystem_finalize_location       = &sampling_subsystem_finalize_location,
    .subsystem_finalize                = &sampling_subsystem_finalize
};
