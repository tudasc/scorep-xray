/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2019,
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
 * Copyright (c) 2009-2013,
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
               SCOREP_MetricProfilingType profilingType,
               SCOREP_MetricHandle        parentHandle );


static void
initialize_metric( SCOREP_MetricDef*          definition,
                   SCOREP_DefinitionManager*  definition_manager,
                   SCOREP_StringHandle        metricNameHandle,
                   SCOREP_StringHandle        descriptionNameHandle,
                   SCOREP_MetricSourceType    sourceType,
                   SCOREP_MetricMode          mode,
                   SCOREP_MetricValueType     valueType,
                   SCOREP_MetricBase          base,
                   int64_t                    exponent,
                   SCOREP_StringHandle        unitNameHandle,
                   SCOREP_MetricProfilingType profilingType,
                   SCOREP_MetricHandle        parentHandle );


static bool
equal_metric( const SCOREP_MetricDef* existingDefinition,
              const SCOREP_MetricDef* newDefinition );


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SCOREP_MetricHandle
SCOREP_Definitions_NewMetric( const char*                name,
                              const char*                description,
                              SCOREP_MetricSourceType    sourceType,
                              SCOREP_MetricMode          mode,
                              SCOREP_MetricValueType     valueType,
                              SCOREP_MetricBase          base,
                              int64_t                    exponent,
                              const char*                unit,
                              SCOREP_MetricProfilingType profilingType,
                              SCOREP_MetricHandle        parentHandle )
{
    UTILS_DEBUG_ENTRY( "%s", name );

    SCOREP_Definitions_Lock();

    SCOREP_MetricHandle new_handle = define_metric(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown metric>", NULL ),
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            description ? description : "", NULL ),
        sourceType,
        mode,
        valueType,
        base,
        exponent,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            unit ? unit : "#", NULL ),
        profilingType,
        parentHandle );

    SCOREP_Definitions_Unlock();


    return new_handle;
}


void
scorep_definitions_unify_metric( SCOREP_MetricDef*             definition,
                                 SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_MetricHandle unified_parent_handle = SCOREP_INVALID_METRIC;
    if ( definition->parent_handle != SCOREP_INVALID_METRIC )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            Metric,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_METRIC,
                      "Invalid unification order of metric definition: parent not yet unified" );
    }

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
        definition->profiling_type,
        unified_parent_handle );
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
               SCOREP_MetricProfilingType profilingType,
               SCOREP_MetricHandle        parentHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_MetricDef*   new_definition = NULL;
    SCOREP_MetricHandle new_handle     = SCOREP_INVALID_METRIC;

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
                       profilingType,
                       parentHandle );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Metric, metric );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_METRIC ) );
    }

    return new_handle;
}


void
initialize_metric( SCOREP_MetricDef*          definition,
                   SCOREP_DefinitionManager*  definition_manager,
                   SCOREP_StringHandle        metricNameHandle,
                   SCOREP_StringHandle        descriptionNameHandle,
                   SCOREP_MetricSourceType    sourceType,
                   SCOREP_MetricMode          mode,
                   SCOREP_MetricValueType     valueType,
                   SCOREP_MetricBase          base,
                   int64_t                    exponent,
                   SCOREP_StringHandle        unitNameHandle,
                   SCOREP_MetricProfilingType profilingType,
                   SCOREP_MetricHandle        parentHandle )
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

    definition->parent_handle = parentHandle;
    if ( definition->parent_handle != SCOREP_INVALID_METRIC )
    {
        HASH_ADD_HANDLE( definition, parent_handle, Metric );
    }
}


bool
equal_metric( const SCOREP_MetricDef* existingDefinition,
              const SCOREP_MetricDef* newDefinition )
{
    return existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->description_handle == newDefinition->description_handle
           && existingDefinition->source_type == newDefinition->source_type
           && existingDefinition->mode == newDefinition->mode
           && existingDefinition->value_type == newDefinition->value_type
           && existingDefinition->base == newDefinition->base
           && existingDefinition->exponent == newDefinition->exponent
           && existingDefinition->unit_handle == newDefinition->unit_handle
           && existingDefinition->parent_handle == newDefinition->parent_handle;
}


/**
 * Returns the sequence number of the unified definitions for a local metric handle from
 * the mappings.
 * @param handle handle to local metric handle.
 */
uint32_t
SCOREP_MetricHandle_GetUnifiedId( SCOREP_MetricHandle handle )
{
    uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
    return scorep_local_definition_manager.metric.mapping[ local_id ];
}


/**
 * Returns the unified handle from a local handle.
 */
SCOREP_MetricHandle
SCOREP_MetricHandle_GetUnified( SCOREP_MetricHandle handle )
{
    return SCOREP_HANDLE_GET_UNIFIED( handle, Metric,
                                      SCOREP_Memory_GetLocalDefinitionPageManager() );
}


/**
 * Returns the value type of a metric.
 * @param handle to local meric definition.
 */
SCOREP_MetricValueType
SCOREP_MetricHandle_GetValueType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->value_type;
}


/**
 * Returns the name of a metric.
 * @param handle to local meric definition.
 */
const char*
SCOREP_MetricHandle_GetName( SCOREP_MetricHandle handle )
{
    SCOREP_MetricDef* metric = SCOREP_LOCAL_HANDLE_DEREF( handle, Metric );

    return SCOREP_LOCAL_HANDLE_DEREF( metric->name_handle, String )->string_data;
}


SCOREP_MetricProfilingType
SCOREP_MetricHandle_GetProfilingType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->profiling_type;
}


SCOREP_MetricHandle
SCOREP_MetricHandle_GetParent( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->parent_handle;
}


/**
 * Returns the mode of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricMode
SCOREP_MetricHandle_GetMode( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->mode;
}

/**
 * Returns the source type of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricSourceType
SCOREP_MetricHandle_GetSourceType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->source_type;
}
