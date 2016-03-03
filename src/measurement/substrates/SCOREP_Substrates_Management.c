/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015-2016,
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
 * This file contains the implementation of substrate management functions.
 *
 */

#include <config.h>

#include <SCOREP_Substrates_Management.h>
#include <scorep_substrates_definition.h>

#include <SCOREP_Subsystem.h>
#include <scorep_status.h>
#include <scorep_properties.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Tracing_Events.h>
#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME SUBSTRATE
#include <UTILS_Debug.h>

#include <inttypes.h>
#include <string.h>

/* *INDENT-OFF* */
static SCOREP_ErrorCode substrates_subsystem_register(size_t);
static SCOREP_ErrorCode substrates_subsystem_init(void);
static SCOREP_ErrorCode substrates_subsystem_init_mpp(void);
static SCOREP_ErrorCode substrates_subsystem_init_location(struct SCOREP_Location*, struct SCOREP_Location*);
static void substrates_subsystem_finalize_location(struct SCOREP_Location*);
static SCOREP_ErrorCode substrates_subsystem_activate_cpu_location(struct SCOREP_Location*, struct SCOREP_Location*, uint32_t, SCOREP_CPULocationPhase);
static void substrates_subsystem_deactivate_cpu_location(struct SCOREP_Location*, struct SCOREP_Location*, SCOREP_CPULocationPhase);
static SCOREP_ErrorCode substrates_subsystem_pre_unify(void);
static SCOREP_ErrorCode substrates_subsystem_post_unify(void);
static void substrates_subsystem_finalize(void);
static void clear_callbacks( void );
static void append_callbacks( const SCOREP_Substrates_Callback* substrateCBs );
/* *INDENT-ON* */


SCOREP_Substrates_Callback scorep_substrates[ SCOREP_SUBSTRATES_NUM_EVENTS ][ SCOREP_SUBSTRATES_NUM_SUBSTRATES + 1 ];

/* ************************************** subsystem struct */

const SCOREP_Subsystem SCOREP_Subsystem_Substrates =
{
    .subsystem_name                    = "SUBSTRATES",
    .subsystem_register                = &substrates_subsystem_register,
    .subsystem_init                    = &substrates_subsystem_init,
    .subsystem_init_mpp                = &substrates_subsystem_init_mpp,
    .subsystem_init_location           = &substrates_subsystem_init_location,
    .subsystem_activate_cpu_location   = &substrates_subsystem_activate_cpu_location,
    .subsystem_deactivate_cpu_location = &substrates_subsystem_deactivate_cpu_location,
    .subsystem_finalize_location       = &substrates_subsystem_finalize_location,
    .subsystem_pre_unify               = &substrates_subsystem_pre_unify,
    .subsystem_post_unify              = &substrates_subsystem_post_unify,
    .subsystem_finalize                = &substrates_subsystem_finalize,
};

static size_t subsystem_id;
static size_t substrate_id = 0;

/* ********************************************* static functions */
static SCOREP_ErrorCode
substrates_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();
    subsystem_id = subsystemId;
    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    clear_callbacks();

    /* SCOREP_EVENT_INIT_SUBSTRATE: needed to set correct substrate id that is
     * used for indexing into location- and task-local substrate data arrays.
     * SCOREP_EVENT_FINALIZE_SUBSTRATE: needed as it provides the substrate id
     * that is needed for reinitializing in OA. */
    if ( SCOREP_IsTracingEnabled() )
    {
        const SCOREP_Substrates_Callback* tracing_callbacks = SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED );
        UTILS_BUG_ON( tracing_callbacks[ SCOREP_EVENT_INIT_SUBSTRATE ] == NULL,
                      "Tracing substrate needs to provide \'SCOREP_EVENT_INIT_SUBSTRATE\' callback." );
        UTILS_BUG_ON( tracing_callbacks[ SCOREP_EVENT_FINALIZE_SUBSTRATE ] == NULL,
                      "Tracing substrate needs to provide \'SCOREP_EVENT_FINALIZE_SUBSTRATE\' callback." );
        append_callbacks( tracing_callbacks );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        const SCOREP_Substrates_Callback* profiling_callbacks = SCOREP_Profile_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED );
        UTILS_BUG_ON( profiling_callbacks[ SCOREP_EVENT_INIT_SUBSTRATE ] == NULL,
                      "Profiling substrate needs to provide \'SCOREP_EVENT_INIT_SUBSTRATE\' callback." );
        UTILS_BUG_ON( profiling_callbacks[ SCOREP_EVENT_FINALIZE_SUBSTRATE ] == NULL,
                      "Profiling substrate needs to provide \'SCOREP_EVENT_FINALIZE_SUBSTRATE\' callback." );
        append_callbacks( profiling_callbacks );
    }

    /* One more substrate to be appended here. */

    substrate_id = 0;

    SCOREP_CALL_SUBSTRATE( InitSubstrate, INIT_SUBSTRATE, ( substrate_id++ ) );

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_init_mpp( void )
{
    SCOREP_CALL_SUBSTRATE( InitializeMpp, INITIALIZE_MPP, ( ) );

    return SCOREP_SUCCESS;
}

/**
 * Clears the the global substrates array scorep_substrates.
 */
static inline void
clear_callbacks( void )
{
    memset( scorep_substrates, 0, sizeof( scorep_substrates ) );
}


/**
 * Appends callbacks from @a substrateCBs to the global substrates
 * array scorep_substrates.
 *
 * @param substrateCBs Array of substrate callbacks to append.
 */
static void
append_callbacks( const SCOREP_Substrates_Callback* substrateCBs )
{
    UTILS_DEBUG_ENTRY();

    for ( uint32_t event = 0; event < SCOREP_SUBSTRATES_NUM_EVENTS; event++ )
    {
        uint32_t substrate = 0;

        while ( scorep_substrates[ event ][ substrate ] && substrate < SCOREP_SUBSTRATES_NUM_SUBSTRATES )
        {
            substrate++;
        }
        UTILS_BUG_ON( substrate == SCOREP_SUBSTRATES_NUM_SUBSTRATES,
                      "scorep_substrates array capacity exceeded "
                      "for event type %" PRIu32 ".\n", event );

        scorep_substrates[ event ][ substrate ] = substrateCBs[ event ];
    }
}


static SCOREP_ErrorCode
substrates_subsystem_init_location( struct SCOREP_Location* location,
                                    struct SCOREP_Location* parent )
{
    UTILS_DEBUG_ENTRY();

    // We are running in parallel here except for the first location.
    // Where to do the locking? Well, at the moment we do the locking
    // in on_location_creation, SCOREP_Tracing_OnLocationCreation.
    // The alternative would be to lock this entire function.
    SCOREP_CALL_SUBSTRATE( OnLocationCreation, ON_LOCATION_CREATION,
                           ( location, parent ) );

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_activate_cpu_location( struct SCOREP_Location* current,
                                            struct SCOREP_Location* parent,
                                            uint32_t                forkSequenceCount,
                                            SCOREP_CPULocationPhase phase )
{
    if ( SCOREP_CPU_LOCATION_PHASE_MGMT != phase )
    {
        return SCOREP_SUCCESS;
    }
    SCOREP_CALL_SUBSTRATE( OnLocationActivation, ON_LOCATION_ACTIVATION,
                           ( current, parent, forkSequenceCount ) );

    return SCOREP_SUCCESS;
}


static void
substrates_subsystem_deactivate_cpu_location( struct SCOREP_Location* current,
                                              struct SCOREP_Location* parent,
                                              SCOREP_CPULocationPhase phase )
{
    if ( SCOREP_CPU_LOCATION_PHASE_MGMT != phase )
    {
        return;
    }
    SCOREP_CALL_SUBSTRATE( OnLocationDeactivation, ON_LOCATION_DEACTIVATION,
                           ( current, parent ) );
}


static void
substrates_subsystem_finalize_location( struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_CALL_SUBSTRATE( OnLocationDeletion, ON_LOCATION_DELETION,
                           ( location ) );
}


static SCOREP_ErrorCode
substrates_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* finalize and close all event writers, release used memory pages. */

    SCOREP_CALL_SUBSTRATE( PreUnifySubstrate, PRE_UNIFY_SUBSTRATE, ( ) );

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_post_unify( void )
{
    UTILS_DEBUG_ENTRY();

    return SCOREP_SUCCESS;
}


static void
substrates_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    /* Ignore return value 'substrate_id', only needed for re-initialization in OA. */
    SCOREP_CALL_SUBSTRATE( FinalizeSubstrate, FINALIZE_SUBSTRATE, ( ) );

    /* One more substrate to be finalized here. */

    substrate_id = 0;
}


void
SCOREP_Substrates_WriteData( void )
{
    UTILS_BUG_ON( !SCOREP_Definitions_Initialized(), "We need valid definitions here." );

    SCOREP_CALL_SUBSTRATE( WriteData, WRITE_DATA, ( ) );
}


void
SCOREP_Substrates_EnableRecording( void )
{
    clear_callbacks();

    if ( SCOREP_IsTracingEnabled() )
    {
        append_callbacks( SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        append_callbacks( SCOREP_Profile_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED ) );
    }

    /* One more substrate to be appended here. */
}


void
SCOREP_Substrates_DisableRecording( void )
{
    clear_callbacks();

    append_callbacks( scorep_properties_get_substrate_callbacks() );

    if ( SCOREP_IsTracingEnabled() )
    {
        append_callbacks( SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_DISABLED ) );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        append_callbacks( SCOREP_Profile_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_DISABLED ) );
    }

    /* One more substrate to be appended here. */
}


size_t
SCOREP_Substrates_NumberOfRegisteredSubstrates( void )
{
    return substrate_id;
}
