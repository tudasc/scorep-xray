/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_METRIC_H
#define SCOREP_PRIVATE_DEFINITIONS_METRIC_H


/**
 * @file
 *
 *
 */

#include <scorep/SCOREP_MetricTypes.h>

SCOREP_DEFINE_DEFINITION_TYPE( Metric )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Metric );

    // Add SCOREP_Metric stuff from here on.
    SCOREP_StringHandle        name_handle;
    SCOREP_StringHandle        description_handle;
    SCOREP_MetricSourceType    source_type;
    SCOREP_MetricMode          mode;
    SCOREP_MetricValueType     value_type;
    SCOREP_MetricBase          base;
    int64_t                    exponent;
    SCOREP_StringHandle        unit_handle;
    SCOREP_MetricProfilingType profiling_type;
    SCOREP_MetricHandle        parent_handle;
};


/**
 * Associate a name with a process unique metric member handle.
 *
 * @param name A meaningful name of the metric member.
 *
 * @return A process unique metric member handle to be used in calls to
 * SCOREP_Definitions_NewSamplingSet().
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
                              SCOREP_MetricHandle        parentHandle );


/**
 * Returns the number of unified metric definitions.
 */
uint32_t
SCOREP_Definitions_GetNumberOfUnifiedMetricDefinitions( void );


/**
 * Returns the sequence number of the unified definitions for a local metric handle from
 * the mappings.
 * @param handle handle to local metric handle.
 */
uint32_t
SCOREP_MetricHandle_GetUnifiedId( SCOREP_MetricHandle handle );


/**
 * Returns the unified handle from a local handle.
 */
SCOREP_MetricHandle
SCOREP_MetricHandle_GetUnified( SCOREP_MetricHandle handle );


/**
 * Returns the value type of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricValueType
SCOREP_MetricHandle_GetValueType( SCOREP_MetricHandle handle );


/**
 * Returns the name of a metric.
 * @param handle to local metric definition.
 */
const char*
SCOREP_MetricHandle_GetName( SCOREP_MetricHandle handle );


/**
 * Returns the profiling type of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricProfilingType
SCOREP_MetricHandle_GetProfilingType( SCOREP_MetricHandle handle );


/**
 * Returns the parent of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricHandle
SCOREP_MetricHandle_GetParent( SCOREP_MetricHandle handle );


/**
 * Returns the mode of a metric.
 * @param handle to local metric definition.
 */
SCOREP_MetricMode
SCOREP_MetricHandle_GetMode( SCOREP_MetricHandle handle );


/**
 * Returns the source type of a metric.(@see SCOREP_MetricTypes.h)
 * @param handle to local metric definition.
 */
SCOREP_MetricSourceType
SCOREP_MetricHandle_GetSourceType( SCOREP_MetricHandle handle );


void
scorep_definitions_unify_metric( SCOREP_MetricDef*                    definition,
                                 struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_METRIC_H */
