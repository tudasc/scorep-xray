/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of the HIP runtime API event logging via HIP callbacks.
 */

#include <config.h>

#include "scorep_hip_callbacks.h"

#include <scorep/SCOREP_PublicTypes.h>

#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_FastHashtab.h>

#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <roctracer_hip.h>

#include <jenkins_hash.h>

#include "scorep_hip.h"
#include "scorep_hip_confvars.h"

// Macro to check ROC-tracer calls status
// Note that this applies only to calls returning `roctracer_status_t`!
#define SCOREP_ROCTRACER_CALL( call ) \
    do { \
        roctracer_status_t err = call; \
        if ( err != ROCTRACER_STATUS_SUCCESS ) { \
            UTILS_FATAL( "Call '%s' failed with: %s", #call, roctracer_error_string() ); \
        } \
    } while ( 0 )

static SCOREP_SourceFileHandle hip_file_handle;

/************************** HIP API function table ****************************/

typedef uint32_t            api_region_table_key_t;
typedef SCOREP_RegionHandle api_region_table_value_t;

#define API_REGION_TABLE_HASH_EXPONENT 7

static inline uint32_t
api_region_table_bucket_idx( api_region_table_key_t key )
{
    uint32_t hashvalue = jenkins_hash( &key, sizeof( key ), 0 );

    return hashvalue & hashmask( API_REGION_TABLE_HASH_EXPONENT );
}

static inline bool
api_region_table_equals( api_region_table_key_t key1,
                         api_region_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
api_region_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
api_region_table_free_chunk( void* chunk )
{
}

typedef struct
{
    SCOREP_RegionType region_type;
    const char*       group_name;
    bool              allow_filter;
} api_region_table_infos;

static inline api_region_table_value_t
api_region_table_value_ctor( api_region_table_key_t* key,
                             void*                   ctorData )
{
    api_region_table_infos* infos = ctorData;
    // `roctracer_op_string` returns at least in one case a static string,
    // thus we cannot reliably free the result
    const char* function_name = roctracer_op_string( ACTIVITY_DOMAIN_HIP_API, *key, 0 );

    SCOREP_RegionHandle region = SCOREP_FILTERED_REGION;
    if ( !infos->allow_filter
         || !SCOREP_Filtering_MatchFunction( function_name, NULL ) )
    {
        region = SCOREP_Definitions_NewRegion( function_name, NULL,
                                               hip_file_handle,
                                               0, 0,
                                               SCOREP_PARADIGM_HIP,
                                               infos->region_type );

        SCOREP_RegionHandle_SetGroup( region, infos->group_name );
    }

    return region;
}

/* nPairsPerChunk: 4+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( api_region_table,
                             15,
                             hashsize( API_REGION_TABLE_HASH_EXPONENT ) );

// Runtime API callback function
static void
api_region_enter( uint32_t          cid,
                  SCOREP_RegionType regionType,
                  const char*       groupName,
                  bool              allowFilter )
{
    api_region_table_infos infos = {
        .region_type  = regionType,
        .group_name   = groupName,
        .allow_filter = allowFilter
    };
    SCOREP_RegionHandle    region_handle = SCOREP_INVALID_REGION;
    api_region_table_get_and_insert( cid, &infos, &region_handle );

    /********** write enter record for HIP driver API **********/
    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( region_handle );
    }
}

static void
api_region_exit( uint32_t cid )
{
    SCOREP_RegionHandle region_handle = SCOREP_INVALID_REGION;
    bool                known         = api_region_table_get( cid, &region_handle );
    UTILS_BUG_ON( !known, "Exit after Enter for unknown region." );

    /********** write exit record for HIP driver API **********/
    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region_handle );
    }
}

static void
api_cb( uint32_t    domain,
        uint32_t    cid,
        const void* callbackData,
        void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_WRAPPER, "HIP_API", !!arg );
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

// Init tracing routine
void
scorep_hip_callbacks_init( void )
{
    UTILS_DEBUG( "######################### Init ROC tracer" );
    UTILS_DEBUG( "    Enabled features: %" PRIx64, scorep_hip_features );

    hip_file_handle = SCOREP_Definitions_NewSourceFile( "HIP" );
}

void
scorep_hip_callbacks_finalize( void )
{
    UTILS_DEBUG( "######################### Finalize ROC tracer" );
}

#define ENABLE_TRACING( fn, callback )      \
    do {                                    \
        SCOREP_ROCTRACER_CALL(              \
            roctracer_enable_op_callback(   \
                ACTIVITY_DOMAIN_HIP_API,    \
                HIP_API_ID_ ## fn,          \
                callback, NULL ) );         \
    } while ( 0 )

// Start tracing routine
void
scorep_hip_callbacks_enable( void )
{
    UTILS_DEBUG( "############################## Start HIP tracing" );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_API )
    {
        /* These are all allowed to be filtered, thus setting callback arg to !NULL */
        SCOREP_ROCTRACER_CALL( roctracer_enable_domain_callback( ACTIVITY_DOMAIN_HIP_API, api_cb, ( void* )1 ) );
    }
}

// Stop tracing routine
void
scorep_hip_callbacks_disable( void )
{
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_callback( ACTIVITY_DOMAIN_HIP_API ) );
    UTILS_DEBUG( "############################## Stop HIP tracing" );
}
