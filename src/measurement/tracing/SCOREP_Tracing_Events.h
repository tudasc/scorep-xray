/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2019,
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


#include <scorep_location_management.h>
#include <scorep_substrates_definition.h>

/**
   Returns an array of tracing callbacks for management events.
   @return Array of tracing substrate callbacks.
 */
const SCOREP_Substrates_Callback*
SCOREP_Tracing_GetSubstrateMgmtCallbacks( void );

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
