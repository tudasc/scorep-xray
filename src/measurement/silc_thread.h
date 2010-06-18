#ifndef SILC_INTERNAL_THREAD_H
#define SILC_INTERNAL_THREAD_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_thread.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include <SILC_Allocator.h>
#include <SILC_Profile.h>
#include <SILC_Thread_Types.h>
#include "silc_trace_thread_interaction.h"
#include <stddef.h>
#include <stdint.h>


/**
 * Call from master thread, e.g. for SILC_InitMeasurement(). Prepare
 * datastructures for thread-local access.
 *
 */
void
SILC_Thread_Initialize();


/**
 * Call from master thread, e.g. SILC_FinalizeMeasurement(). Cleans up thread
 * local data structures. Who clears memory and when?
 *
 */
void
SILC_Thread_Finalize();


/**
 * Call from SILC_OmpFork(). Update thread local data structures at the start of
 * a parallel region.
 *
 */
void
SILC_Thread_OnThreadFork( size_t nRequestedThreads );


/**
 * Call from SILC_OmpJoin(). Update thread local data structures at the end of
 * a parallel region.
 *
 */
void
SILC_Thread_OnThreadJoin();


/**
 *
 *
 * @return
 */
SILC_Thread_LocationData*
SILC_Thread_GetLocationData();


SILC_Allocator_PageManager**
SILC_Thread_GetLocationLocalMemoryPageManagers();


SILC_Allocator_PageManager**
SILC_Thread_GetGlobalMemoryPageManagers();


SILC_Profile_LocationData*
SILC_Thread_GetProfileLocationData( SILC_Thread_LocationData* locationData );


SILC_Trace_LocationData*
SILC_Thread_GetTraceLocationData( SILC_Thread_LocationData* locationData );


uint64_t
SILC_Thread_GetLocationId( SILC_Thread_LocationData* locationData );

SILC_LocationHandle
SILC_Thread_GetLocationHandle( SILC_Thread_LocationData* locationData );


uint32_t
SILC_Thread_GetNumberOfLocations();


#endif /* SILC_INTERNAL_THREAD_H */
