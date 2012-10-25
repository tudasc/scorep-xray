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


// temporary to get nesting running. nesting_level will be replaced by
// fork_count (in generic threading) to match parent- and child-threads.
uint32_t
scorep_thread_get_nesting_level();


#endif /* SCOREP_INTERNAL_THREAD_H */
