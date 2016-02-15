/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_ALLOC_METRIC_H
#define SCOREP_ALLOC_METRIC_H

/**
 * @file        SCOREP_AllocMetric.h
 *
 * @brief
 */


#include <stdlib.h>
#include <stdint.h>


#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>

struct SCOREP_Location;

typedef struct SCOREP_AllocMetric SCOREP_AllocMetric;


SCOREP_ErrorCode
SCOREP_AllocMetric_New( const char*          name,
                        SCOREP_AllocMetric** allocMetric );


void
SCOREP_AllocMetric_Destroy( SCOREP_AllocMetric* allocMetric );


/**
 *  Handles an allocation in this metric.
 *
 *  @param allocMetric   Object handle.
 *  @param resultAddr    The resulting address of the allocation.
 *  @param size          The size of the allocation.
 */
void
SCOREP_AllocMetric_HandleAlloc( SCOREP_AllocMetric* allocMetric,
                                uint64_t            resultAddr,
                                size_t              size );

/**
 *  Handles an reallocation in this metric.
 *
 *  @param allocMetric   Object handle.
 *  @param resultAddr    The resulting address of the allocation.
 *  @param size          The size of the allocation.
 *  @param prevAddr      In case of an re-allocation the previous pointer.
 *  @param[out] prevSize The size of the previous allocation if @a prevAddr is
 *                       not @p NULL, or 0. May be a NULL pointer.
 */
void
SCOREP_AllocMetric_HandleRealloc( SCOREP_AllocMetric* allocMetric,
                                  uint64_t            resultAddr,
                                  size_t              size,
                                  uint64_t            prevAddr,
                                  uint64_t*           prevSize );


/**
 *  Handles an deallocation in this metric.
 *
 *  @param allocMetric   Object handle.
 *  @param addr          The of the allocation to be freed.
 *  @param[out] size     The size of this allocation. May be a NULL pointer.
 */
void
SCOREP_AllocMetric_HandleFree( SCOREP_AllocMetric* allocMetric,
                               uint64_t            addr,
                               uint64_t*           size );


/**
 *  Triggers SCOREP_LeakedMemory events for allocations that weren't freed.
 *  Supposed to be used at subsystem_end time. Clears the the data structure
 *  that is used for pointer tracking.
 *
 *  @param allocMetric   Object handle.
 */
void
SCOREP_AllocMetric_ReportLeaked( SCOREP_AllocMetric* allocMetric );


SCOREP_AttributeHandle
SCOREP_AllocMetric_GetAllocationSizeAttribute( void );


SCOREP_AttributeHandle
SCOREP_AllocMetric_GetDeallocationSizeAttribute( void );


#endif /* SCOREP_ALLOC_METRIC_H */
