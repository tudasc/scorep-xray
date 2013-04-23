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
 * @file       src/measurement/definitions/scorep_definitions_metric.c
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
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_MetricHandle
define_metric( SCOREP_DefinitionManager*  definition_manager,
               SCOREP_StringHandle        metricNameHandle,
               SCOREP_StringHandle        descriptionNameHandle,
               SCOREP_MetricSourceType    sourceType,
               SCOREP_MetricMode          mode,
               SCOREP_MetricValueType     valueType,
               SCOREP_MetricBase          base,
               int64_t                    exponent,
               SCOREP_StringHandle        unitNameHandle,
               SCOREP_MetricProfilingType profilingType );


static void
initialize_metric( SCOREP_Metric_Definition*  definition,
                   SCOREP_DefinitionManager*  definition_manager,
                   SCOREP_StringHandle        metricNameHandle,
                   SCOREP_StringHandle        descriptionNameHandle,
                   SCOREP_MetricSourceType    sourceType,
                   SCOREP_MetricMode          mode,
                   SCOREP_MetricValueType     valueType,
                   SCOREP_MetricBase          base,
                   int64_t                    exponent,
                   SCOREP_StringHandle        unitNameHandle,
                   SCOREP_MetricProfilingType profilingType );


static bool
equal_metric( const SCOREP_Metric_Definition* existingDefinition,
              const SCOREP_Metric_Definition* newDefinition );


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SCOREP_MetricHandle
SCOREP_DefineMetric( const char*                name,
                     const char*                description,
                     SCOREP_MetricSourceType    sourceType,
                     SCOREP_MetricMode          mode,
                     SCOREP_MetricValueType     valueType,
                     SCOREP_MetricBase          base,
                     int64_t                    exponent,
                     const char*                unit,
                     SCOREP_MetricProfilingType profilingType )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_Metric_Definition* new_definition = NULL;
    SCOREP_MetricHandle       new_handle     = define_metric(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "<unknown metric>" ),
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            description ? description : "" ),
        sourceType,
        mode,
        valueType,
        base,
        exponent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            unit ? unit : "#" ),
        profilingType );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyMetricDefinitionToUnified( SCOREP_Metric_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

    definition->unified = define_metric(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->description_handle,
            String,
            handlesPageManager ),
        definition->source_type,
        definition->mode,
        definition->value_type,
        definition->base,
        definition->exponent,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->unit_handle,
            String,
            handlesPageManager ),
        definition->profiling_type );
}


SCOREP_MetricHandle
define_metric( SCOREP_DefinitionManager*  definition_manager,
               SCOREP_StringHandle        metricNameHandle,
               SCOREP_StringHandle        descriptionNameHandle,
               SCOREP_MetricSourceType    sourceType,
               SCOREP_MetricMode          mode,
               SCOREP_MetricValueType     valueType,
               SCOREP_MetricBase          base,
               int64_t                    exponent,
               SCOREP_StringHandle        unitNameHandle,
               SCOREP_MetricProfilingType profilingType )
{
    assert( definition_manager );

    SCOREP_Metric_Definition* new_definition = NULL;
    SCOREP_MetricHandle       new_handle     = SCOREP_INVALID_METRIC;

    SCOREP_DEFINITION_ALLOC( Metric );
    initialize_metric( new_definition,
                       definition_manager,
                       metricNameHandle,
                       descriptionNameHandle,
                       sourceType,
                       mode,
                       valueType,
                       base,
                       exponent,
                       unitNameHandle,
                       profilingType );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Metric, metric );

    return new_handle;
}


void
initialize_metric( SCOREP_Metric_Definition*  definition,
                   SCOREP_DefinitionManager*  definition_manager,
                   SCOREP_StringHandle        metricNameHandle,
                   SCOREP_StringHandle        descriptionNameHandle,
                   SCOREP_MetricSourceType    sourceType,
                   SCOREP_MetricMode          mode,
                   SCOREP_MetricValueType     valueType,
                   SCOREP_MetricBase          base,
                   int64_t                    exponent,
                   SCOREP_StringHandle        unitNameHandle,
                   SCOREP_MetricProfilingType profilingType )
{
    definition->name_handle = metricNameHandle;
    HASH_ADD_HANDLE( definition, name_handle, String );

    definition->description_handle = descriptionNameHandle;
    HASH_ADD_HANDLE( definition, description_handle, String );

    definition->source_type = sourceType;
    HASH_ADD_POD( definition, source_type );

    definition->mode = mode;
    HASH_ADD_POD( definition, mode );

    definition->value_type = valueType;
    HASH_ADD_POD( definition, value_type );

    definition->base = base;
    HASH_ADD_POD( definition, base );

    definition->exponent = exponent;
    HASH_ADD_POD( definition, exponent );

    definition->unit_handle = unitNameHandle;
    HASH_ADD_HANDLE( definition, unit_handle, String );

    definition->profiling_type = profilingType;
    HASH_ADD_POD( definition, profiling_type );
}


bool
equal_metric( const SCOREP_Metric_Definition* existingDefinition,
              const SCOREP_Metric_Definition* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->description_handle == newDefinition->description_handle
           && existingDefinition->source_type == newDefinition->source_type
           && existingDefinition->mode == newDefinition->mode
           && existingDefinition->value_type == newDefinition->value_type
           && existingDefinition->base == newDefinition->base
           && existingDefinition->exponent == newDefinition->exponent
           && existingDefinition->unit_handle == newDefinition->unit_handle;
}


/**
 * Returns the sequence number of the unified definitions for a local metric handle from
 * the mappings.
 * @param handle handle to local metric handle.
 */
uint32_t
SCOREP_Metric_GetUnifiedSequenceNumber( SCOREP_MetricHandle handle )
{
    uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
    return scorep_local_definition_manager.mappings->metric_mappings[ local_id ];
}


/**
 * Returns the unified handle from a local handle.
 */
SCOREP_MetricHandle
SCOREP_Metric_GetUnifiedHandle( SCOREP_MetricHandle handle )
{
    return SCOREP_HANDLE_GET_UNIFIED( handle, Metric,
                                      SCOREP_Memory_GetLocalDefinitionPageManager() );
}


/**
 * Returns the number of unified metric definitions.
 */
uint32_t
SCOREP_Metric_GetNumberOfUnifiedDefinitions( void )
{
    return scorep_unified_definition_manager->metric_definition_counter;
}


/**
 * Returns the value type of a metric.
 * @param handle to local meric definition.
 */
SCOREP_MetricValueType
SCOREP_Metric_GetValueType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->value_type;
}


/**
 * Returns the name of a metric.
 * @param handle to local meric definition.
 */
const char*
SCOREP_Metric_GetName( SCOREP_MetricHandle handle )
{
    SCOREP_Metric_Definition* metric = SCOREP_LOCAL_HANDLE_DEREF( handle, Metric );

    return SCOREP_LOCAL_HANDLE_DEREF( metric->name_handle, String )->string_data;
}


SCOREP_MetricProfilingType
SCOREP_Metric_GetProfilingType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->profiling_type;
}