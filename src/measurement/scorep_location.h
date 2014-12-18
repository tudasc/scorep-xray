/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 */

#ifndef SCOREP_INTERNAL_LOCATION_H
#define SCOREP_INTERNAL_LOCATION_H


#include <stdint.h>
#include <stdbool.h>
#include <SCOREP_Location.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Profile.h>
#include <tracing/SCOREP_Tracing_ThreadInteraction.h>

void
SCOREP_Location_Initialize( void );


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up
 * locations data structures.
 *
 * Must be called before SCOREP_Thread_Finalize().
 */
void
SCOREP_Location_Finalize( void );


SCOREP_Location*
SCOREP_Location_CreateCPULocation( const char* name );

/**
 * Returns the current number of locations.
 */
uint32_t
SCOREP_Location_GetCountOfLocations( void );


SCOREP_Allocator_PageManager*
SCOREP_Location_GetMemoryPageManager( SCOREP_Location*  locationData,
                                      SCOREP_MemoryType type );


SCOREP_Profile_LocationData*
SCOREP_Location_GetProfileData( SCOREP_Location* locationData );


SCOREP_TracingData*
SCOREP_Location_GetTracingData( SCOREP_Location* locationData );


/*
 * The caller must ensure thread safety. Currently called at measurement
 * time for the current location at flush time, and at finalization time.
 */
void
SCOREP_Location_EnsureGlobalId( SCOREP_Location* location );


void
SCOREP_Location_FinalizeDefinitions( void );


void
SCOREP_Location_FinalizeLocations( void );


void
SCOREP_Location_CallSubstratesOnNewLocation( SCOREP_Location* locationData,
                                             SCOREP_Location* parent );


void
SCOREP_Location_CallSubstratesOnActivation( SCOREP_Location* current,
                                            SCOREP_Location* parent,
                                            uint32_t         forkSequenceCount );


void
SCOREP_Location_CallSubstratesOnDeactivation( SCOREP_Location* current,
                                              SCOREP_Location* parent );


#endif /* SCOREP_INTERNAL_LOCATION_H */
