/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_INTERNAL_THREAD_H
#define SCOREP_INTERNAL_THREAD_H



/**
 * @file       scorep_thread.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <scorep_utility/SCOREP_Allocator.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Thread_Types.h>
#include <tracing/SCOREP_Tracing_ThreadInteraction.h>
#include <stddef.h>
#include <stdint.h>


/**
 * Call from master thread, e.g. for SCOREP_InitMeasurement(). Prepare
 * datastructures for thread-local access.
 *
 */
void
SCOREP_Thread_Initialize();


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up thread
 * private data structures. Who clears memory and when?
 *
 */
void
SCOREP_Thread_Finalize();


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up
 * locations data structures.
 *
 * Must be called before SCOREP_Thread_Finalize().
 */
void
SCOREP_Thread_FinalizeLocations();


/**
 * Call from SCOREP_OmpFork(). Update thread local data structures at the start of
 * a parallel region.
 *
 */
void
SCOREP_Thread_OnThreadFork( size_t nRequestedThreads );


/**
 * Call from SCOREP_OmpJoin(). Update thread local data structures at the end of
 * a parallel region.
 *
 */
void
SCOREP_Thread_OnThreadJoin();


SCOREP_Allocator_PageManager**
SCOREP_Thread_GetLocationLocalMemoryPageManagers();


SCOREP_Profile_LocationData*
SCOREP_Thread_GetProfileLocationData( SCOREP_Thread_LocationData* locationData );


SCOREP_Trace_LocationData*
SCOREP_Thread_GetTraceLocationData( SCOREP_Thread_LocationData* locationData );


uint64_t
SCOREP_Thread_GetGlobalLocationId( SCOREP_Thread_LocationData* locationData );


SCOREP_LocationHandle
SCOREP_Thread_GetLocationHandle( SCOREP_Thread_LocationData* locationData );

uint64_t
SCOREP_Thread_GetLastTimestamp( SCOREP_Thread_LocationData* locationData );

void
SCOREP_Thread_SetLastTimestamp( SCOREP_Thread_LocationData* locationData,
                                int64_t                     timestamp );


uint32_t
SCOREP_Thread_GetNumberOfLocations();


void
SCOREP_ProcessDeferredLocations();


#endif /* SCOREP_INTERNAL_THREAD_H */
