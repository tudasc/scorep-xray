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
 * @file
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
 *  Acquires a previous allocation.
 *
 *  Locates a previous allocation of address @p addr and removes it from the
 *  the tracking. The allocation must than be passed to @a SCOREP_AllocMetric_HandleFree
 *  or @a SCOREP_AllocMetric_HandleRealloc to finalize the handling.
 *
 *  @param allocMetric      Object handle.
 *  @param addr             The address of the allocation.
 *  @param[out] allocation  The allocation.
 */
void
SCOREP_AllocMetric_AcquireAlloc( SCOREP_AllocMetric* allocMetric,
                                 uint64_t            addr,
                                 void**              allocation );

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
 *  @param allocMetric     Object handle.
 *  @param resultAddr      The resulting address of the allocation.
 *  @param size            The size of the allocation.
 *  @param prevAllocation  In case of an re-allocation the previous allocation
 *                         returned by @a SCOREP_AllocMetric_AcquireAlloc.
 *  @param[out] prevSize   The size of the previous allocation if @a prevAddr is
 *                         not @p NULL, or 0. May be a NULL pointer.
 */
void
SCOREP_AllocMetric_HandleRealloc( SCOREP_AllocMetric* allocMetric,
                                  uint64_t            resultAddr,
                                  size_t              size,
                                  void*               prevAllocation,
                                  uint64_t*           prevSize );


/**
 *  Handles an deallocation in this metric.
 *
 *  @param allocMetric   Object handle.
 *  @param allocation    The allocation to be freed. Returned from
 *                       @a SCOREP_AllocMetric_AcquireAlloc.
 *  @param[out] size     The size of this allocation. May be a NULL pointer.
 */
void
SCOREP_AllocMetric_HandleFree( SCOREP_AllocMetric* allocMetric,
                               void*               allocation,
                               uint64_t*           size );


/**
 *  Triggers SCOREP_LeakedMemory events for allocations that weren't freed.
 *  Supposed to be used at subsystem_end time. Clears the data structure
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
