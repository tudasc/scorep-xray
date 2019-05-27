/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Tracing_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_SamplingSetHandle
define_sampling_set( SCOREP_DefinitionManager*     definition_manager,
                     uint8_t                       numberOfMetrics,
                     const SCOREP_MetricHandle*    metrics,
                     SCOREP_MetricOccurrence       occurrence,
                     SCOREP_SamplingSetClass       klass,
                     SCOREP_Allocator_PageManager* handlesPageManager );


static void
initialize_sampling_set( SCOREP_SamplingSetDef*        definition,
                         SCOREP_DefinitionManager*     definition_manager,
                         uint8_t                       numberOfMetrics,
                         const SCOREP_MetricHandle*    metrics,
                         SCOREP_MetricOccurrence       occurrence,
                         SCOREP_SamplingSetClass       klass,
                         SCOREP_Allocator_PageManager* handlesPageManager );


static SCOREP_SamplingSetHandle
define_scoped_sampling_set( SCOREP_DefinitionManager* definition_manager,
                            SCOREP_SamplingSetHandle  samplingSet,
                            SCOREP_LocationHandle     recorderHandle,
                            SCOREP_MetricScope        scopeType,
                            SCOREP_AnyHandle          scopeHandle );


static void
initialize_scoped_sampling_set( SCOREP_ScopedSamplingSetDef* definition,
                                SCOREP_DefinitionManager*    definition_manager,
                                SCOREP_SamplingSetHandle     samplingSet,
                                SCOREP_LocationHandle        recorderHandle,
                                SCOREP_MetricScope           scopeType,
                                SCOREP_AnyHandle             scopeHandle );


static bool
equal_sampling_set( const SCOREP_SamplingSetDef* existingDefinition,
                    const SCOREP_SamplingSetDef* newDefinition );


SCOREP_SamplingSetHandle
SCOREP_Definitions_NewSamplingSet( uint8_t                    numberOfMetrics,
                                   const SCOREP_MetricHandle* metrics,
                                   SCOREP_MetricOccurrence    occurrence,
                                   SCOREP_SamplingSetClass    klass )
{
    UTILS_DEBUG_ENTRY( "#%hhu metrics", numberOfMetrics );

    SCOREP_Definitions_Lock();

    SCOREP_SamplingSetHandle new_handle = define_sampling_set(
        &scorep_local_definition_manager,
        numberOfMetrics,
        metrics,
        occurrence,
        klass,
        NULL /* take the metric handles as is */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_SamplingSetHandle
SCOREP_Definitions_NewScopedSamplingSet( SCOREP_SamplingSetHandle samplingSet,
                                         SCOREP_LocationHandle    recorderHandle,
                                         SCOREP_MetricScope       scopeType,
                                         SCOREP_AnyHandle         scopeHandle )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_SamplingSetHandle new_handle = define_scoped_sampling_set(
        &scorep_local_definition_manager,
        samplingSet,
        recorderHandle,
        scopeType,
        scopeHandle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_SamplingSetHandle
SCOREP_SamplingSetHandle_GetSamplingSet( SCOREP_SamplingSetHandle samplingSet )
{
    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );

    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set
            = ( SCOREP_ScopedSamplingSetDef* )sampling_set;

        return scoped_sampling_set->sampling_set_handle;
    }

    return samplingSet;
}


void
scorep_definitions_unify_sampling_set( SCOREP_SamplingSetDef*        definition,
                                       SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition
            = ( SCOREP_ScopedSamplingSetDef* )definition;

        UTILS_BUG_ON( scoped_definition->sampling_set_handle == SCOREP_INVALID_SAMPLING_SET,
                      "Invalid scoped sampling set definition: missing sampling set" );
        UTILS_BUG_ON( scoped_definition->recorder_handle == SCOREP_INVALID_LOCATION,
                      "Invalid scoped sampling set definition: missing location" );
        UTILS_BUG_ON( scoped_definition->scope_handle == SCOREP_MOVABLE_NULL,
                      "Invalid scoped sampling set definition: missing scope" );

        definition->unified = define_scoped_sampling_set(
            scorep_unified_definition_manager,
            SCOREP_HANDLE_GET_UNIFIED(
                scoped_definition->sampling_set_handle,
                SamplingSet,
                handlesPageManager ),
            SCOREP_HANDLE_GET_UNIFIED(
                scoped_definition->recorder_handle,
                Location,
                handlesPageManager ),
            scoped_definition->scope_type,
            SCOREP_HANDLE_GET_UNIFIED(
                scoped_definition->scope_handle,
                Any,
                handlesPageManager ) );
    }
    else
    {
        definition->unified = define_sampling_set(
            scorep_unified_definition_manager,
            definition->number_of_metrics,
            definition->metric_handles,
            definition->occurrence,
            definition->klass,
            handlesPageManager /* take the unified handles from the metrics */ );
    }
}


SCOREP_SamplingSetHandle
define_sampling_set( SCOREP_DefinitionManager*     definition_manager,
                     uint8_t                       numberOfMetrics,
                     const SCOREP_MetricHandle*    metrics,
                     SCOREP_MetricOccurrence       occurrence,
                     SCOREP_SamplingSetClass       klass,
                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_SamplingSetDef*   new_definition = NULL;
    SCOREP_SamplingSetHandle new_handle     = SCOREP_INVALID_SAMPLING_SET;

    size_t size_for_sampling_set = SCOREP_Allocator_RoundupToAlignment(
        sizeof( SCOREP_SamplingSetDef ) +
        ( ( numberOfMetrics ) * sizeof( SCOREP_MetricHandle ) ) );
    if ( !handlesPageManager )
    {
        size_for_sampling_set += SCOREP_Tracing_GetSamplingSetCacheSize( numberOfMetrics );
    }

    SCOREP_DEFINITION_ALLOC_SIZE( SamplingSet, size_for_sampling_set );

    initialize_sampling_set( new_definition,
                             definition_manager,
                             numberOfMetrics,
                             metrics,
                             occurrence,
                             klass,
                             handlesPageManager );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SamplingSet, sampling_set );

    if ( !handlesPageManager )
    {
        size_for_sampling_set               -= SCOREP_Tracing_GetSamplingSetCacheSize( numberOfMetrics );
        new_definition->tracing_cache_offset = size_for_sampling_set;
        SCOREP_Tracing_CacheSamplingSet( new_handle );
    }

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_SAMPLING_SET ) );
    }

    return new_handle;
}


void
initialize_sampling_set( SCOREP_SamplingSetDef*        definition,
                         SCOREP_DefinitionManager*     definition_manager,
                         uint8_t                       numberOfMetrics,
                         const SCOREP_MetricHandle*    metrics,
                         SCOREP_MetricOccurrence       occurrence,
                         SCOREP_SamplingSetClass       klass,
                         SCOREP_Allocator_PageManager* handlesPageManager )
{
    definition->is_scoped = false;
    HASH_ADD_POD( definition, is_scoped );

    /* not unify relevant */
    definition->tracing_cache_offset = 0;

    definition->number_of_metrics = numberOfMetrics;
    HASH_ADD_POD( definition, number_of_metrics );

    /* Copy array of metrics */
    if ( handlesPageManager )
    {
        for ( uint8_t i = 0; i < numberOfMetrics; i++ )
        {
            definition->metric_handles[ i ] =
                SCOREP_HANDLE_GET_UNIFIED( metrics[ i ],
                                           Metric,
                                           handlesPageManager );
            UTILS_BUG_ON( definition->metric_handles[ i ] == SCOREP_INVALID_METRIC,
                          "Invalid metric reference in sampling set definition" );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }
    else
    {
        for ( uint8_t i = 0; i < numberOfMetrics; i++ )
        {
            definition->metric_handles[ i ] = metrics[ i ];
            UTILS_BUG_ON( definition->metric_handles[ i ] == SCOREP_INVALID_METRIC,
                          "Invalid metric reference in sampling set definition" );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }

    definition->occurrence = occurrence;
    HASH_ADD_POD( definition, occurrence );

    definition->klass = klass;
    HASH_ADD_POD( definition, klass );

    definition->recorders      = SCOREP_INVALID_SAMPLING_SET_RECORDER;
    definition->recorders_tail = &definition->recorders;
}


SCOREP_SamplingSetHandle
define_scoped_sampling_set( SCOREP_DefinitionManager* definition_manager,
                            SCOREP_SamplingSetHandle  samplingSet,
                            SCOREP_LocationHandle     recorderHandle,
                            SCOREP_MetricScope        scopeType,
                            SCOREP_AnyHandle          scopeHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_ScopedSamplingSetDef* new_definition = NULL;
    SCOREP_SamplingSetHandle     new_handle     = SCOREP_INVALID_SAMPLING_SET;

    SCOREP_DEFINITION_ALLOC( ScopedSamplingSet );
    initialize_scoped_sampling_set( new_definition,
                                    definition_manager,
                                    samplingSet,
                                    recorderHandle,
                                    scopeType,
                                    scopeHandle );

    SCOREP_ScopedSamplingSetDef* scoped_definition = new_definition;
    SCOREP_SamplingSetHandle     scoped_handle     = new_handle;

    /* ScopedSamplingSet overloads SamplingSet */
    {
        SCOREP_SamplingSetDef* new_definition
            = ( SCOREP_SamplingSetDef* )scoped_definition;
        SCOREP_SamplingSetHandle new_handle = scoped_handle;

        /* Does return if it is a duplicate */
        SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SamplingSet,
                                                   sampling_set );

        if ( definition_manager == &scorep_local_definition_manager )
        {
            SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                        ( new_handle, SCOREP_HANDLE_TYPE_SAMPLING_SET ) );
        }
        return new_handle;
    }
}


void
initialize_scoped_sampling_set( SCOREP_ScopedSamplingSetDef* definition,
                                SCOREP_DefinitionManager*    definition_manager,
                                SCOREP_SamplingSetHandle     samplingSet,
                                SCOREP_LocationHandle        recorderHandle,
                                SCOREP_MetricScope           scopeType,
                                SCOREP_AnyHandle             scopeHandle )
{
    definition->is_scoped = true;
    HASH_ADD_POD( definition, is_scoped );

    definition->sampling_set_handle = samplingSet;
    HASH_ADD_HANDLE( definition, sampling_set_handle, SamplingSet );

    definition->recorder_handle = recorderHandle;
    HASH_ADD_HANDLE( definition, recorder_handle, Location );

    definition->scope_type = scopeType;
    HASH_ADD_POD( definition, scope_type );

    definition->scope_handle = scopeHandle;
    HASH_ADD_HANDLE( definition, scope_handle, Any );
}


bool
equal_sampling_set( const SCOREP_SamplingSetDef* existingDefinition,
                    const SCOREP_SamplingSetDef* newDefinition )
{
    if ( existingDefinition->is_scoped != newDefinition->is_scoped )
    {
        return false;
    }

    if ( existingDefinition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* existing_scoped_definition
            = ( SCOREP_ScopedSamplingSetDef* )existingDefinition;
        SCOREP_ScopedSamplingSetDef* new_scoped_definition
            = ( SCOREP_ScopedSamplingSetDef* )newDefinition;
        return existing_scoped_definition->sampling_set_handle
               == new_scoped_definition->sampling_set_handle
               && existing_scoped_definition->recorder_handle
               == new_scoped_definition->recorder_handle
               && existing_scoped_definition->scope_type
               == new_scoped_definition->scope_type
               && existing_scoped_definition->scope_handle
               == new_scoped_definition->scope_handle;
    }
    else
    {
        return existingDefinition->number_of_metrics
               == newDefinition->number_of_metrics
               && 0 == memcmp( existingDefinition->metric_handles,
                               newDefinition->metric_handles,
                               sizeof( existingDefinition->metric_handles[ 0 ] )
                               * existingDefinition->number_of_metrics  )
               && existingDefinition->occurrence
               == newDefinition->occurrence;
    }
}

uint8_t
SCOREP_SamplingSetHandle_GetNumberOfMetrics( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition = ( SCOREP_ScopedSamplingSetDef* )definition;
        definition = SCOREP_LOCAL_HANDLE_DEREF( scoped_definition->sampling_set_handle, SamplingSet );
    }
    return definition->number_of_metrics;
}

const SCOREP_MetricHandle*
SCOREP_SamplingSetHandle_GetMetricHandles( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition = ( SCOREP_ScopedSamplingSetDef* )definition;
        definition = SCOREP_LOCAL_HANDLE_DEREF( scoped_definition->sampling_set_handle, SamplingSet );
    }
    return definition->metric_handles;
}

bool
SCOREP_SamplingSetHandle_IsScoped( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    return definition->is_scoped;
}

SCOREP_MetricScope
SCOREP_SamplingSetHandle_GetScope( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition = ( SCOREP_ScopedSamplingSetDef* )definition;
        return scoped_definition->scope_type;
    }
    return SCOREP_INVALID_METRIC_SCOPE;
}

SCOREP_MetricOccurrence
SCOREP_SamplingSetHandle_GetMetricOccurrence( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition = ( SCOREP_ScopedSamplingSetDef* )definition;
        definition = SCOREP_LOCAL_HANDLE_DEREF( scoped_definition->sampling_set_handle, SamplingSet );
    }
    return definition->occurrence;
}

SCOREP_SamplingSetClass
SCOREP_SamplingSetHandle_GetSamplingSetClass( SCOREP_SamplingSetHandle handle )
{
    SCOREP_SamplingSetDef* definition = SCOREP_LOCAL_HANDLE_DEREF( handle, SamplingSet );
    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_definition = ( SCOREP_ScopedSamplingSetDef* )definition;
        definition = SCOREP_LOCAL_HANDLE_DEREF( scoped_definition->sampling_set_handle, SamplingSet );
    }
    return definition->klass;
}
