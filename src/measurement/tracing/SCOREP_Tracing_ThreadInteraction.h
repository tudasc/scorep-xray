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


#ifndef SCOREP_TRACING_THREAD_INTERACTION_H
#define SCOREP_TRACING_THREAD_INTERACTION_H


/**
 * @file       src/measurement/tracing/SCOREP_Tracing_ThreadInteraction.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 */


#include <SCOREP_Location.h>


typedef struct SCOREP_Trace_LocationData SCOREP_Trace_LocationData;


/**
 * Allocate a valid SCOREP_Trace_LocationData object, initialize it in
 * SCOREP_Tracing_OnLocationCreation().
 *
 */
SCOREP_Trace_LocationData*
SCOREP_Tracing_CreateLocationData();


/**
 * Clean up @a traceLocationData at the end of a phase or at the end of the
 * measurement.
 *
 * @param traceLocationData The object to be deleted
 */
void
SCOREP_Tracing_DeleteLocationData( SCOREP_Trace_LocationData* traceLocationData );


/**
 * Triggered on thread creation, i.e. when a thread is encountered the first
 * time. Note that several thread can share teh same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Tracing_OnThreadCreation( SCOREP_Location* locationData,
                                 SCOREP_Location* parentLocationData );

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
SCOREP_Tracing_OnThreadActivation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData );


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
SCOREP_Tracing_OnThreadDectivation( SCOREP_Location* locationData,
                                    SCOREP_Location* parentLocationData );


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share teh same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Tracing_OnLocationCreation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData );


void
SCOREP_Tracing_AssignLocationId( SCOREP_Location* threadLocationData );


#endif /* SCOREP_TRACING_THREAD_INTERACTION_H */
