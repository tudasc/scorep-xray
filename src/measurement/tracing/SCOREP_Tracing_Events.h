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
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_TRACING_EVENTS_H
#define SCOREP_TRACING_EVENTS_H


/**
 * @file
 *
 * @brief Event consumption functions for tracing.
 */


#include <stdint.h>
#include <stdbool.h>


#include <scorep_location.h>
#include <scorep_substrates_definition.h>


/**
 * Records metrics for tracing. This function stores dense metrics for tracing.
 *
 * @param location     A pointer to the thread location data of the thread that executed
 *                     the metric event.
 * @param timestamp    The timestamp, when the metric event occurred.
 * @param regionHandle The handle of the region for which the metric event occurred.
 * @param metricValues Array of the dense metric values.
 *
 */
void
SCOREP_Tracing_Metric( SCOREP_Location*         location,
                       uint64_t                 timestamp,
                       SCOREP_SamplingSetHandle samplingSet,
                       const uint64_t*          metricValues );


/**
   Returns an array of tracing callbacks.
   @param mode Recording enabled/disabled.
   @return Array of tracing substrate callbacks for the requested mode.
 */
const SCOREP_Substrates_Callback*
SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode );


size_t
SCOREP_Tracing_GetSamplingSetCacheSize( uint32_t numberOfMetrics );


void
SCOREP_Tracing_CacheSamplingSet( SCOREP_SamplingSetHandle samplingSet );


#endif /* SCOREP_TRACING_EVENTS_H */
