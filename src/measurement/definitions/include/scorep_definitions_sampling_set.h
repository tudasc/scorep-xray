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

#ifndef SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H
#define SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( SamplingSet )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    /* offset after this struct to the caching for tracing */
    size_t tracing_cache_offset;

    SCOREP_MetricOccurrence occurrence;
    SCOREP_SamplingSetClass klass;

    /* List of SamplingSetRecorderHandles for this sampling set. */
    SCOREP_SamplingSetRecorderHandle  recorders;
    SCOREP_SamplingSetRecorderHandle* recorders_tail;

    uint8_t             number_of_metrics;
    SCOREP_MetricHandle metric_handles[];
};


/*
 * this is not a real definition
 * but an overload of the SamplingSet definition
 * because they share the same id namespace in OTF2
 */
SCOREP_DEFINE_DEFINITION_TYPE( ScopedSamplingSet )
{
    // Don't use ScopedSamplingSet here, there is no handle for this definition
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    SCOREP_SamplingSetHandle sampling_set_handle;
    SCOREP_LocationHandle    recorder_handle;
    SCOREP_MetricScope       scope_type;
    /* all types are handles */
    SCOREP_AnyHandle scope_handle;
};


/**
 * Define a new sampling set.
 *
 * @param numberOfMetrics Number of metrics contained in array @a metrics.
 * @param metrics         Array containing metric handles of all members
 *                        of this sampling set.
 * @param occurrence      Specifies whether a metric occurs at every region enter and leave
 *                        (SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT), only at a region
 *                        enter and leave but does not need to occur at every enter/leave
 *                        (SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS) or at any place i.e. it is
 *                        not related to region enter or leaves (SCOREP_METRIC_OCCURRENCE_ASYNCHRONOUS).
 *
 * @return A process unique sampling set handle to be used in calls to
 * SCOREP_Definitions_NewScopedSamplingSet().
 */
SCOREP_SamplingSetHandle
SCOREP_Definitions_NewSamplingSet( uint8_t                    numberOfMetrics,
                                   const SCOREP_MetricHandle* metrics,
                                   SCOREP_MetricOccurrence    occurrence,
                                   SCOREP_SamplingSetClass    klass );


/**
 * Define a new scoped sampling set. The scoped sampling set is recorded by
 * a location specified by @a recorderHandle. In contrast to a normal
 * <em>sampling set</em> the values of a scoped sampling set are valid for
 * another location or a group of locations.
 *
 * @param samplingSet    Handle of a previously defined sampling set.
 * @param recorderHandle Handle of the location that records this sampling set.
 * @param scopeType      Defines whether the scope of the sampling set is another
 *                       location (SCOREP_METRIC_SCOPE_LOCATION), a location group
 *                       (SCOREP_METRIC_SCOPE_LOCATION_GROUP), a system tree node
 *                       (SCOREP_METRIC_SCOPE_SYSTEM_TREE_NODE) or a generic group
 *                       of locations (SCOREP_METRIC_SCOPE_GROUP).
 * @param scopeHandle    Handle of the sampling set scope according to @a scopeType.
 */
SCOREP_SamplingSetHandle
SCOREP_Definitions_NewScopedSamplingSet( SCOREP_SamplingSetHandle samplingSet,
                                         SCOREP_LocationHandle    recorderHandle,
                                         SCOREP_MetricScope       scopeType,
                                         SCOREP_AnyHandle         scopeHandle );


/**
 * Adds a recorder to a sampling set.
 *
 * The class of the sampling set must be in {CPU, GPU} and the type of the
 * location must match this class.
 */
void
SCOREP_SamplingSet_AddRecorder( SCOREP_SamplingSetHandle samplingSetHandle,
                                SCOREP_LocationHandle    recorderHandle );


/**
 * A SCOREP_SamplingSetHandle can refer to a normal sampling set or a scoped
 * one. A scoped sampling set contains a reference to the underlying sampling
 * set, which contains all metric members needed to record their values.
 *
 * This function handles differences between normal and scoped sampling sets and
 * returns always a handle of a sampling set.
 *
 * @param samplingSet   Handle of a normal or scoped sampling set.
 *
 * @return Returns handle of a normal sampling set.
 */
SCOREP_SamplingSetHandle
SCOREP_SamplingSetHandle_GetSamplingSet( SCOREP_SamplingSetHandle samplingSet );


void
scorep_definitions_unify_sampling_set( SCOREP_SamplingSetDef*               definition,
                                       struct SCOREP_Allocator_PageManager* handlesPageManager );


/**
 * Get the number of metrics in a sampling set.
 * @param handle the handle to the existing sampling set
 * @return the number of associated metrics
 */
uint8_t
SCOREP_SamplingSetHandle_GetNumberOfMetrics( SCOREP_SamplingSetHandle handle );

/**
 * Get the metric handles in a sampling set.
 * @param handle the handle to the existing sampling set
 * @return a list of associated metrics. get the length of the list with SCOREP_SamplingSet_GetNumberOfMetrics
 */
const SCOREP_MetricHandle*
SCOREP_SamplingSetHandle_GetMetricHandles( SCOREP_SamplingSetHandle handle );

/**
 * Check whether a sampling set is scoped (belongs to a number of locations).
 * @param handle the handle to the existing sampling set
 * @return whether the sampling set is scoped or not
 */
bool
SCOREP_SamplingSetHandle_IsScoped( SCOREP_SamplingSetHandle handle );


/**
 * Returns the scope of the sampling set or SCOREP_INVALID_METRIC_SCOPE if sampling set is not scoped
 * @param handle the handle to the existing sampling set
 * @return scope or SCOREP_INVALID_METRIC_SCOPE
 */
SCOREP_MetricScope
SCOREP_SamplingSetHandle_GetScope( SCOREP_SamplingSetHandle handle );

/**
 * Returns the metric occurence of the sampling set
 * @param handle the handle to the existing sampling set
 * @return metric occurence
 */
SCOREP_MetricOccurrence
SCOREP_SamplingSetHandle_GetMetricOccurrence( SCOREP_SamplingSetHandle handle );

/**
 * Returns the class of the sampling set
 * @param handle the handle to the existing sampling set
 * @return sampling set class
 */
SCOREP_SamplingSetClass
SCOREP_SamplingSetHandle_GetSamplingSetClass( SCOREP_SamplingSetHandle handle );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H */
