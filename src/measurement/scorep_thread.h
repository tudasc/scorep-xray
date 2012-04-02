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
#include <SCOREP_Memory.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Location.h>
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


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up
 * locations data structures.
 *
 * Must be called before SCOREP_Thread_Finalize().
 */
void
SCOREP_Location_Finalize();


SCOREP_Allocator_PageManager*
SCOREP_Location_GetMemoryPageManager( SCOREP_Location*  locationData,
                                      SCOREP_MemoryType type );


SCOREP_Profile_LocationData*
SCOREP_Location_GetProfileData( SCOREP_Location* locationData );


SCOREP_TracingData*
SCOREP_Location_GetTracingData( SCOREP_Location* locationData );


uint64_t
SCOREP_Location_GetGlobalId( SCOREP_Location* locationData );


void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  int64_t          timestamp );


void
SCOREP_Location_ProcessDeferredOnes();


#endif /* SCOREP_INTERNAL_THREAD_H */
