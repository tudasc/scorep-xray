/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen, Germany
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

/**
 * @file       src/measurement/scorep_location.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
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
SCOREP_Location_Initialize();


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up
 * locations data structures.
 *
 * Must be called before SCOREP_Thread_Finalize().
 */
void
SCOREP_Location_Finalize();


/** @param  deferNewLocationNotication tpd-based thread implementations must notify,
 * i.e. call SCOREP_Location_CallSubstratesOnNewLocation(), not before the tpd update
 * to ensure that tpd is in a valid state. */
SCOREP_Location*
SCOREP_Location_CreateCPULocation( SCOREP_Location* parent,
                                   const char*      name,
                                   bool             deferNewLocationNotication );


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
SCOREP_Location_CloseDeferredDefinitions();


void
SCOREP_Location_CallSubstratesOnNewLocation( SCOREP_Location* locationData,
                                             const char*      name,
                                             SCOREP_Location* parent );


void
SCOREP_Location_CallSubstratesOnActivation( SCOREP_Location* current,
                                            SCOREP_Location* parent,
                                            uint32_t         forkSequenceCount );


void
SCOREP_Location_CallSubstratesOnDeactivation( SCOREP_Location* current,
                                              SCOREP_Location* parent );


#endif /* SCOREP_INTERNAL_LOCATION_H */
