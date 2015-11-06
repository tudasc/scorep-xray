/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_TRACING_THREAD_INTERACTION_H
#define SCOREP_TRACING_THREAD_INTERACTION_H


/**
 * @file
 *
 */


#include <SCOREP_Location.h>


struct SCOREP_TracingData;


/**
 * Allocate a valid SCOREP_TracingData object, initialize it in
 * SCOREP_Tracing_OnLocationCreation().
 *
 */
struct SCOREP_TracingData*
SCOREP_Tracing_CreateLocationData( SCOREP_Location* locationData );


/**
 * Clean up the location specific trace data at the end of a phase or at the
 * end of the measurement.
 *
 * @param location The location object holding the trace data to be deleted
 */
void
SCOREP_Tracing_DeleteLocationData( SCOREP_Location* location );


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share the same location data.
 *
 * @param locationData       Location data of the current thread.
 * @param parentLocationData Unused.
 */
void
SCOREP_Tracing_OnLocationCreation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData );

/**
 * Triggered on
 *
 * @param threadLocationData Location data of the current thread.
 */
void
SCOREP_Tracing_AssignLocationId( SCOREP_Location* threadLocationData );

#endif /* SCOREP_TRACING_THREAD_INTERACTION_H */
