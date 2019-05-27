/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015-2016, 2019,
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
#include <scorep_properties_management.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Tracing_Events.h>
#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME SUBSTRATE
#include <UTILS_Debug.h>

#include <inttypes.h>
#include <string.h>


#include "scorep_substrates_plugins.h"

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
static void substrates_subsystem_dump_manifest(FILE* manifestFile, const char* relativeSourceDir, const char* targetDir);
/* *INDENT-ON* */

/* Used from elsewhere, initialized in SCOREP_Substrates_EarlyInitialize/substrate_pack */
const SCOREP_Substrates_Callback * scorep_substrates = NULL;
uint32_t scorep_substrates_max_substrates = 0;

SCOREP_Substrates_Callback* scorep_substrates_mgmt                = NULL;
uint32_t                    scorep_substrates_max_mgmt_substrates = 0;

/* internal stuff used to switch between enabled and disabled */
static SCOREP_Substrates_Callback* substrates_enabled;
static uint32_t                    max_substrates_enabled;
static SCOREP_Substrates_Callback* substrates_disabled;
static uint32_t                    max_substrates_disabled;

/* ************************************** subsystem struct */

const SCOREP_Subsystem SCOREP_Subsystem_Substrates =
{
    .subsystem_name                    = "SUBSTRATE",
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
    .subsystem_dump_manifest           = &substrates_subsystem_dump_manifest,
};

static size_t subsystem_id;
static size_t substrate_id;


#define CACHE_LINE_SIZE 64

static void*
aligned_malloc( size_t size )
{
    /* includes up to cache line size + 8 byte additional data */
    void* super = malloc( size + CACHE_LINE_SIZE + sizeof( void* ) );

    UTILS_BUG_ON( super == NULL,
                  "scorep_substrates array could not be allocated " );

    /* create aligned address */
    void** aligned = ( void** )( ( uintptr_t )( super + CACHE_LINE_SIZE + sizeof( void* ) ) & ( uintptr_t ) ~( CACHE_LINE_SIZE - 1 ) );

    /* store original address */
    aligned[ -1 ] = super;

    /* return aligned address */
    return aligned;
}

static void
aligned_free( void* alignedAdr )
{
    /* free original address that prepends the aligned one */
    free( ( ( void** )alignedAdr )[ -1 ] );
}

/* ********************************************* static functions */
static SCOREP_ErrorCode
substrates_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();
    subsystem_id = subsystemId;

    SCOREP_Substrate_Plugins_Register();

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* for re-initialization in OA */
    substrate_id = 0;

    SCOREP_CALL_SUBSTRATE_MGMT( InitSubstrate, INIT_SUBSTRATE, ( substrate_id++ ) );

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
substrates_subsystem_init_mpp( void )
{
    SCOREP_CALL_SUBSTRATE_MGMT( InitializeMpp, INITIALIZE_MPP, ( ) );

    return SCOREP_SUCCESS;
}


/**
 * Appends callbacks of a specific substrate to the unpacked list of callbacks
 * @param incomingCBs The callbacks from a specific substrate
 * @param allCbs the unpacked list of all callbacks, must hold maxLength*nr_substrates entries
 * @length the length of incomingCBs as the substrate defines it
 * @maxLength the length of incomingCBs as this Score-P version defines it, i.e. SCOREP_SUBSTRATES_NUM_MGMT_EVENTS or SCOREP_SUBSTRATES_NUM_EVENTS
 * @nr_substrates the number of registered substrates that was also used to create allCbs
 */
static void
append_callbacks( const SCOREP_Substrates_Callback* incomingCBs,
                  SCOREP_Substrates_Callback*       allCBs,
                  uint32_t                          length,
                  uint32_t                          maxLength,
                  int                               nrSubstrates )
{
    UTILS_DEBUG_ENTRY();

    if ( length > maxLength )
    {
        length = maxLength;
    }

    for ( uint32_t event = 0; event < length; event++ )
    {
        if ( incomingCBs[ event ] != NULL )
        {
            uint32_t substrate = 0;

            /* search for NULL terminator */
            while ( allCBs[ event * ( nrSubstrates + 1 ) + substrate ] )
            {
                substrate++;
            }

            /* append substrate event */
            allCBs[ event * ( nrSubstrates + 1 ) + substrate ] = incomingCBs[ event ];
        }
    }
}

static SCOREP_ErrorCode
substrates_subsystem_init_location( struct SCOREP_Location* location,
                                    struct SCOREP_Location* parent )
{
    UTILS_DEBUG_ENTRY();

    /* allocate data before calling plugins */
    SCOREP_Substrate_Plugins_InitLocationData( location );

    // We are running in parallel here except for the first location.
    // Where to do the locking? Well, at the moment we do the locking
    // in on_location_creation, SCOREP_Tracing_OnLocationCreation.
    // The alternative would be to lock this entire function.
    SCOREP_CALL_SUBSTRATE_MGMT( OnLocationCreation, ON_LOCATION_CREATION,
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

    SCOREP_CALL_SUBSTRATE_MGMT( OnCpuLocationActivation, ON_CPU_LOCATION_ACTIVATION,
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

    SCOREP_CALL_SUBSTRATE_MGMT( OnCpuLocationDeactivation, ON_CPU_LOCATION_DEACTIVATION,
                                ( current, parent ) );
}


static void
substrates_subsystem_finalize_location( struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_CALL_SUBSTRATE_MGMT( OnLocationDeletion, ON_LOCATION_DELETION,
                                ( location ) );
}


static SCOREP_ErrorCode
substrates_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* finalize and close all event writers, release used memory pages. */

    SCOREP_CALL_SUBSTRATE_MGMT( PreUnifySubstrate, PRE_UNIFY_SUBSTRATE, ( ) );

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
    SCOREP_CALL_SUBSTRATE_MGMT( FinalizeSubstrate, FINALIZE_SUBSTRATE, ( ) );

    substrate_id = 0;

    aligned_free( substrates_enabled );
    aligned_free( substrates_disabled );
    aligned_free( scorep_substrates_mgmt );
}

static void
substrates_subsystem_dump_manifest( FILE*       manifestFile,
                                    const char* relativeSourceDir,
                                    const char* targetDir )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_CALL_SUBSTRATE_MGMT( DumpManifest, DUMP_MANIFEST,
                                ( manifestFile, relativeSourceDir, targetDir ) );
}

void
SCOREP_Substrates_WriteData( void )
{
    UTILS_BUG_ON( !SCOREP_Definitions_Initialized(), "We need valid definitions here." );

    SCOREP_CALL_SUBSTRATE_MGMT( WriteData, WRITE_DATA, ( ) );
}


void
SCOREP_Substrates_EnableRecording( void )
{
    scorep_substrates                = substrates_enabled;
    scorep_substrates_max_substrates = max_substrates_enabled;
}


void
SCOREP_Substrates_DisableRecording( void )
{
    scorep_substrates                = substrates_disabled;
    scorep_substrates_max_substrates = max_substrates_disabled;
}

static void
substrates_pack( SCOREP_Substrates_Callback*  source,
                 uint32_t                     sourceMainLength,
                 SCOREP_Substrates_Callback** target,
                 uint32_t*                    targetSubLength,
                 uint32_t                     nrSubstrates )
{
    uint32_t source_it, sub, max = 0;
    /* get longest setting in source */
    for ( source_it = 0; source_it < sourceMainLength; source_it++ )
    {
        sub = 0;
        while ( source[ source_it * ( nrSubstrates + 1 ) + sub ] != NULL )
        {
            sub++;
        }
        if ( sub > max )
        {
            max = sub;
        }
    }

    max += 1;

    /* should be aligned to a half cache line. Thus it should be 1, 2, 4, or match a half 64 byte cache line */
    switch ( max )
    {
        case 1: /* none -> 8 events per cache line ((Null), (Null), ..., (Null)) */
            break;
        case 2:
            /* one substrate -> 4 events per cache line ((foo,NULL), (bar,NULL), (baz,NULL), (bat,NULL))*/
            break;
        case 4:
            /* three substrate -> 2 events per cache line ((foo,foo2,foo3,NULL), (bar,bar2,bar3,NULL))*/
            break;
        default:
            /* make sure one event spans over full or half cache lines, assuming 64 byte cache line */
            switch ( max * sizeof( void ( * )( void ) ) % ( CACHE_LINE_SIZE / 2 ) )
            {
                case 0: /* already the case */
                    break;
                default:
                {
                    /* fix alignment to 32 byte */
                    int diff = ( CACHE_LINE_SIZE / 2 ) / sizeof( void ( * )( void ) ) - ( max * sizeof( void ( * )( void ) ) % ( CACHE_LINE_SIZE / 2 ) ) / sizeof( void ( * )( void ) );
                    max = max + diff;
                    break;
                }
            }
    }

    *targetSubLength = max;

    /* allocate and fill target */
    *target = aligned_malloc( sourceMainLength * max * sizeof( SCOREP_Substrates_Callback ) );

    UTILS_BUG_ON( ( *target == NULL ), "Could not allocate memory for substrate callbacks." );

    for ( source_it = 0; source_it < sourceMainLength; source_it++ )
    {
        sub = 0;
        while ( source[ source_it * ( nrSubstrates + 1 ) + sub ] != NULL )
        {
            ( *target )[ source_it * ( *targetSubLength ) + sub ] = source[ source_it * ( nrSubstrates + 1 ) + sub ];
            sub++;
        }
        ( *target )[ source_it * ( *targetSubLength ) + sub ] = NULL;
    }
}


void
SCOREP_Substrates_EarlyInitialize( void )
{
    /* Initialize plugins */
    SCOREP_Substrate_Plugins_EarlyInit();

    /* get max. number of substrates, start with 1 for properties */
    int nr_of_substrates = 1;
    if ( SCOREP_IsTracingEnabled() )
    {
        nr_of_substrates++;
    }
    if ( SCOREP_IsProfilingEnabled() )
    {
        nr_of_substrates++;
    }
    nr_of_substrates += SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins();

    /* initialize unpacked data structures */
    SCOREP_Substrates_Callback* substrates_enabled_unpacked     = calloc( ( nr_of_substrates + 1 ) * SCOREP_SUBSTRATES_NUM_EVENTS, sizeof( SCOREP_Substrates_Callback ) );
    SCOREP_Substrates_Callback* substrates_disabled_unpacked    = calloc( ( nr_of_substrates + 1 ) * SCOREP_SUBSTRATES_NUM_EVENTS, sizeof( SCOREP_Substrates_Callback ) );
    SCOREP_Substrates_Callback* scorep_substrates_mgmt_unpacked = calloc( ( nr_of_substrates + 1 ) * SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, sizeof( SCOREP_Substrates_Callback ) );

    /* historically, properties were the first things in the disabled list, so add it first */
    append_callbacks( scorep_properties_get_substrate_callbacks(), substrates_disabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
    append_callbacks( scorep_properties_get_substrate_mgmt_callbacks(), scorep_substrates_mgmt_unpacked, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, nr_of_substrates );


    /* SCOREP_EVENT_INIT_SUBSTRATE: needed to set correct substrate id that is
     * used for indexing into location- and task-local substrate data arrays.
     * SCOREP_EVENT_FINALIZE_SUBSTRATE: needed as it provides the substrate id
     * that is needed for reinitializing in OA. */
    if ( SCOREP_IsTracingEnabled() )
    {
        const SCOREP_Substrates_Callback* tracing_callbacks          = SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED );
        const SCOREP_Substrates_Callback* tracing_callbacks_disabled = SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_DISABLED );
        const SCOREP_Substrates_Callback* tracing_mgmt_callbacks     = SCOREP_Tracing_GetSubstrateMgmtCallbacks();
        UTILS_BUG_ON( tracing_mgmt_callbacks[ SCOREP_MGMT_INIT_SUBSTRATE ] == NULL,
                      "Tracing substrate needs to provide \'SCOREP_EVENT_INIT_SUBSTRATE\' callback." );
        UTILS_BUG_ON( tracing_mgmt_callbacks[ SCOREP_MGMT_FINALIZE_SUBSTRATE ] == NULL,
                      "Tracing substrate needs to provide \'SCOREP_EVENT_FINALIZE_SUBSTRATE\' callback." );

        append_callbacks( tracing_callbacks, substrates_enabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
        append_callbacks( tracing_callbacks_disabled, substrates_disabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
        append_callbacks( tracing_mgmt_callbacks, scorep_substrates_mgmt_unpacked, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, nr_of_substrates );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        const SCOREP_Substrates_Callback* profiling_callbacks          = SCOREP_Profile_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_ENABLED );
        const SCOREP_Substrates_Callback* profiling_callbacks_disabled = SCOREP_Profile_GetSubstrateCallbacks( SCOREP_SUBSTRATES_RECORDING_DISABLED );
        const SCOREP_Substrates_Callback* profiling_mgmt_callbacks     = SCOREP_Profile_GetSubstrateMgmtCallbacks();
        UTILS_BUG_ON( profiling_mgmt_callbacks[ SCOREP_MGMT_INIT_SUBSTRATE ] == NULL,
                      "Profiling substrate needs to provide \'SCOREP_EVENT_INIT_SUBSTRATE\' callback." );
        UTILS_BUG_ON( profiling_mgmt_callbacks[ SCOREP_MGMT_FINALIZE_SUBSTRATE ] == NULL,
                      "Profiling substrate needs to provide \'SCOREP_EVENT_FINALIZE_SUBSTRATE\' callback." );

        append_callbacks( profiling_callbacks, substrates_enabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
        append_callbacks( profiling_callbacks_disabled, substrates_disabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
        append_callbacks( profiling_mgmt_callbacks, scorep_substrates_mgmt_unpacked, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, nr_of_substrates );
    }

    /* for SCOREP_SUBSTRATES_RECORDING_ENABLED */
    SCOREP_Substrates_Mode mode = SCOREP_SUBSTRATES_RECORDING_ENABLED;

    uint32_t                    current_plugin   = 0;
    SCOREP_Substrates_Callback* callbacks        = NULL;
    uint32_t                    callbacks_length = 0;
    while ( ( current_plugin = SCOREP_Substrate_Plugins_GetSubstrateCallbacks( mode, current_plugin, &callbacks, &callbacks_length ) ) != 0 )
    {
        if ( callbacks_length > SCOREP_SUBSTRATES_NUM_EVENTS )
        {
            bool uses_more_functions = false;
            for ( uint32_t index = SCOREP_SUBSTRATES_NUM_EVENTS; index < callbacks_length; index++ )
            {
                if ( callbacks[ index ] )
                {
                    uses_more_functions = true;
                }
            }
            if ( uses_more_functions )
            {
                UTILS_WARNING( "Substrate plugin nr %d provides more event functions than supported by Score-P for SCOREP_Substrates_Mode = SCOREP_SUBSTRATES_RECORDING_ENABLED", current_plugin - 1 );
            }
        }
        append_callbacks( callbacks, substrates_enabled_unpacked, callbacks_length, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
    }

    /* for SCOREP_SUBSTRATES_RECORDING_DISABLED */
    mode           = SCOREP_SUBSTRATES_RECORDING_DISABLED;
    current_plugin = 0;
    while ( ( current_plugin = SCOREP_Substrate_Plugins_GetSubstrateCallbacks( mode, current_plugin, &callbacks, &callbacks_length ) ) != 0 )
    {
        if ( callbacks_length > SCOREP_SUBSTRATES_NUM_EVENTS )
        {
            bool uses_more_functions = false;
            for ( uint32_t index = SCOREP_SUBSTRATES_NUM_EVENTS; index < callbacks_length; index++ )
            {
                if ( callbacks[ index ] )
                {
                    uses_more_functions = true;
                }
            }
            if ( uses_more_functions )
            {
                UTILS_WARNING( "Substrate plugin nr %d provides more event functions than supported by Score-P for SCOREP_Substrates_Mode = SCOREP_SUBSTRATES_RECORDING_DISABLED", current_plugin - 1 );
            }
        }
        append_callbacks( callbacks, substrates_disabled_unpacked, callbacks_length, SCOREP_SUBSTRATES_NUM_EVENTS, nr_of_substrates );
    }

    /* for management */
    current_plugin = 0;
    while ( ( current_plugin = SCOREP_Substrate_Plugins_GetSubstrateMgmtCallbacks( current_plugin, &callbacks ) ) != 0 )
    {
        append_callbacks( callbacks, scorep_substrates_mgmt_unpacked, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, nr_of_substrates );
        free( callbacks );
    }

    /* pack the enabled/disabled/management */
    substrates_pack( substrates_enabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, &substrates_enabled, &max_substrates_enabled, nr_of_substrates );
    substrates_pack( substrates_disabled_unpacked, SCOREP_SUBSTRATES_NUM_EVENTS, &substrates_disabled, &max_substrates_disabled, nr_of_substrates );
    substrates_pack( scorep_substrates_mgmt_unpacked, SCOREP_SUBSTRATES_NUM_MGMT_EVENTS, &scorep_substrates_mgmt, &scorep_substrates_max_mgmt_substrates, nr_of_substrates );

    /* free the unpacked data structures */
    free( substrates_enabled_unpacked );
    free( substrates_disabled_unpacked );
    free( scorep_substrates_mgmt_unpacked );

    /* initially, we are enabled */
    scorep_substrates                = substrates_enabled;
    scorep_substrates_max_substrates = max_substrates_enabled;
}


uint32_t
SCOREP_Substrates_NumberOfRegisteredSubstrates( void )
{
    return substrate_id;
}
