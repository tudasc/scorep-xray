/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @status     alpha
 * @file       src/measurement/definitions/scorep_definitions_sampling_set.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
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
#include <scorep_types.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_SamplingSetHandle
define_sampling_set( SCOREP_DefinitionManager*     definition_manager,
                     uint8_t                       numberOfMetrics,
                     const SCOREP_MetricHandle*    metrics,
                     SCOREP_MetricOccurrence       occurrence,
                     SCOREP_Allocator_PageManager* handlesPageManager );


static void
initialize_sampling_set( SCOREP_SamplingSet_Definition* definition,
                         SCOREP_DefinitionManager*      definition_manager,
                         uint8_t                        numberOfMetrics,
                         const SCOREP_MetricHandle*     metrics,
                         SCOREP_MetricOccurrence        occurrence,
                         SCOREP_Allocator_PageManager*  handlesPageManager );


static SCOREP_SamplingSetHandle
define_scoped_sampling_set( SCOREP_DefinitionManager* definition_manager,
                            SCOREP_SamplingSetHandle  samplingSet,
                            SCOREP_LocationHandle     recorderHandle,
                            SCOREP_MetricScope        scopeType,
                            SCOREP_AnyHandle          scopeHandle );


static void
initialize_scoped_sampling_set( SCOREP_ScopedSamplingSet_Definition* definition,
                                SCOREP_DefinitionManager*            definition_manager,
                                SCOREP_SamplingSetHandle             samplingSet,
                                SCOREP_LocationHandle                recorderHandle,
                                SCOREP_MetricScope                   scopeType,
                                SCOREP_AnyHandle                     scopeHandle );


static bool
equal_sampling_set( const SCOREP_SamplingSet_Definition* existingDefinition,
                    const SCOREP_SamplingSet_Definition* newDefinition );


SCOREP_SamplingSetHandle
SCOREP_DefineSamplingSet( uint8_t                    numberOfMetrics,
                          const SCOREP_MetricHandle* metrics,
                          SCOREP_MetricOccurrence    occurrence )
{
    UTILS_DEBUG_ENTRY( "#%hhu metrics", numberOfMetrics );

    SCOREP_Definitions_Lock();

    SCOREP_SamplingSetHandle new_handle = define_sampling_set(
        &scorep_local_definition_manager,
        numberOfMetrics,
        metrics,
        occurrence,
        NULL /* take the metric handles as is */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_SamplingSetHandle
SCOREP_DefineScopedSamplingSet( SCOREP_SamplingSetHandle samplingSet,
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
SCOREP_GetSamplingSet( SCOREP_SamplingSetHandle samplingSet )
{
    SCOREP_SamplingSet_Definition* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );

    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* scoped_sampling_set
            = ( SCOREP_ScopedSamplingSet_Definition* )sampling_set;

        return scoped_sampling_set->sampling_set_handle;
    }

    return samplingSet;
}


void
SCOREP_CopySamplingSetDefinitionToUnified( SCOREP_SamplingSet_Definition* definition,
                                           SCOREP_Allocator_PageManager*  handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )definition;

        assert( scoped_definition->sampling_set_handle != SCOREP_INVALID_SAMPLING_SET );
        assert( scoped_definition->recorder_handle != SCOREP_INVALID_LOCATION );
        assert( scoped_definition->scope_handle != SCOREP_MOVABLE_NULL );

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
            handlesPageManager /* take the unified handles from the metrics */ );
    }
}


SCOREP_SamplingSetHandle
define_sampling_set( SCOREP_DefinitionManager*     definition_manager,
                     uint8_t                       numberOfMetrics,
                     const SCOREP_MetricHandle*    metrics,
                     SCOREP_MetricOccurrence       occurrence,
                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition_manager );

    SCOREP_SamplingSet_Definition* new_definition = NULL;
    SCOREP_SamplingSetHandle       new_handle     = SCOREP_INVALID_SAMPLING_SET;

    size_t size_for_sampling_set = SCOREP_Allocator_RoundupToAlignment(
        sizeof( SCOREP_SamplingSet_Definition ) +
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
                             handlesPageManager );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SamplingSet, sampling_set );

    if ( !handlesPageManager )
    {
        size_for_sampling_set               -= SCOREP_Tracing_GetSamplingSetCacheSize( numberOfMetrics );
        new_definition->tracing_cache_offset = size_for_sampling_set;
        SCOREP_Tracing_CacheSamplingSet( new_handle );
    }

    return new_handle;
}


void
initialize_sampling_set( SCOREP_SamplingSet_Definition* definition,
                         SCOREP_DefinitionManager*      definition_manager,
                         uint8_t                        numberOfMetrics,
                         const SCOREP_MetricHandle*     metrics,
                         SCOREP_MetricOccurrence        occurrence,
                         SCOREP_Allocator_PageManager*  handlesPageManager )
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
            assert( definition->metric_handles[ i ]
                    != SCOREP_INVALID_METRIC );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }
    else
    {
        for ( uint8_t i = 0; i < numberOfMetrics; i++ )
        {
            definition->metric_handles[ i ] = metrics[ i ];
            assert( definition->metric_handles[ i ]
                    != SCOREP_INVALID_METRIC );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }

    definition->occurrence = occurrence;
    HASH_ADD_POD( definition, occurrence );
}


SCOREP_SamplingSetHandle
define_scoped_sampling_set( SCOREP_DefinitionManager* definition_manager,
                            SCOREP_SamplingSetHandle  samplingSet,
                            SCOREP_LocationHandle     recorderHandle,
                            SCOREP_MetricScope        scopeType,
                            SCOREP_AnyHandle          scopeHandle )
{
    assert( definition_manager );

    SCOREP_ScopedSamplingSet_Definition* new_definition = NULL;
    SCOREP_SamplingSetHandle             new_handle     = SCOREP_INVALID_SAMPLING_SET;

    SCOREP_DEFINITION_ALLOC( ScopedSamplingSet );
    initialize_scoped_sampling_set( new_definition,
                                    definition_manager,
                                    samplingSet,
                                    recorderHandle,
                                    scopeType,
                                    scopeHandle );

    SCOREP_ScopedSamplingSet_Definition* scoped_definition = new_definition;
    SCOREP_SamplingSetHandle             scoped_handle     = new_handle;

    /* ScopedSamplingSet overloads SamplingSet */
    {
        SCOREP_SamplingSet_Definition* new_definition
            = ( SCOREP_SamplingSet_Definition* )scoped_definition;
        SCOREP_SamplingSetHandle new_handle = scoped_handle;

        /* Does return if it is a duplicate */
        SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SamplingSet,
                                                  sampling_set );

        return new_handle;
    }
}


void
initialize_scoped_sampling_set( SCOREP_ScopedSamplingSet_Definition* definition,
                                SCOREP_DefinitionManager*            definition_manager,
                                SCOREP_SamplingSetHandle             samplingSet,
                                SCOREP_LocationHandle                recorderHandle,
                                SCOREP_MetricScope                   scopeType,
                                SCOREP_AnyHandle                     scopeHandle )
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
equal_sampling_set( const SCOREP_SamplingSet_Definition* existingDefinition,
                    const SCOREP_SamplingSet_Definition* newDefinition )
{
    if ( existingDefinition->is_scoped != newDefinition->is_scoped )
    {
        return false;
    }

    if ( existingDefinition->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* existing_scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )existingDefinition;
        SCOREP_ScopedSamplingSet_Definition* new_scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )newDefinition;
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
                               sizeof( existingDefinition->metric_handles[ 0 ]
                                       * existingDefinition->number_of_metrics ) )
               && existingDefinition->occurrence
               == newDefinition->occurrence;
    }
}