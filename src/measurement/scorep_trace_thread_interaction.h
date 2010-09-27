/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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

#ifndef SCOREP_INTERNAL_TRACE_THREADINTERACTION_H
#define SCOREP_INTERNAL_TRACE_THREADINTERACTION_H



/**
 * @file       SCOREP_Trace_ThreadInteraction.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include "scorep_trace_types.h"
#include <SCOREP_Thread_Types.h>



/**
 * Allocate a valid SCOREP_Trace_LocationData object, initialize it in
 * SCOREP_Trace_OnLocationCreation().
 *
 */
SCOREP_Trace_LocationData*
SCOREP_Trace_CreateLocationData();


/**
 * Clean up @a traceLocationData at the end of a phase or at the end of the
 * measurement.
 *
 * @param traceLocationData The object to be deleted
 */
void
SCOREP_Trace_DeleteLocationData( SCOREP_Trace_LocationData* traceLocationData );


/**
 * Triggered on thread creation, i.e. when a thread is encountered the first
 * time. Note that several thread can share teh same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Trace_OnThreadCreation( SCOREP_Thread_LocationData* locationData,
                               SCOREP_Thread_LocationData* parentLocationData );

/**
 * Triggered at the start of every thread/parallel region. Always triggered,
 * even after thread creation. In contrast to creation this function may be
 * triggered multiple times, e.g. if we reenter a parallel region again or if
 * we reuse the location/thread in a different parallel region.
 *
 * @param locationData Location data of the current thread inside the paralell
 * region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Trace_OnThreadActivation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData );


/**
 * Triggered after the end of every thread/parallel region, i.e. in the join
 * event.
 *
 * @param locationData Location data of the deactivated thread inside the
 * parallel region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Trace_OnThreadDectivation( SCOREP_Thread_LocationData* locationData,
                                  SCOREP_Thread_LocationData* parentLocationData );


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share teh same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Trace_OnLocationCreation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData );


void
SCOREP_SetOtf2WriterLocationId( SCOREP_Thread_LocationData* threadLocationData );


#endif /* SCOREP_INTERNAL_TRACE_THREADINTERACTION_H */
