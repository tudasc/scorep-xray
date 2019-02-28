/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2018
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_LOCATION_H
#define SCOREP_LOCATION_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "SCOREP_Types.h"
#include <SCOREP_DefinitionHandles.h>

/**
 * @file
 *
 *
 *
 */


typedef struct SCOREP_Location SCOREP_Location;


/**
 * This function can be used by subsystems to create new locations.
 *
 * @param parent            Handle of parent location.
 * @param type              Type of new location.
 * @param name              Name of new location.
 *
 * @return Returns handle for new location.
 */
SCOREP_Location*
SCOREP_Location_CreateNonCPULocation( SCOREP_Location*    parent,
                                      SCOREP_LocationType type,
                                      const char*         name );


/**
 * Returns a metric location. Into this location all asynchronous
 * metrics of a process will be recorded. As this location is shared among
 * adapters, this function acquires a mutex that need to be released by a call
 * to SCOREP_Location_ReleasePerProcessMetricsLocation(). The location is
 * created during the first call to this function.
 * If @a timestamp is non NULL, than also takes a timestamp for this location
 * and return it in @a *timestamp.
 */
SCOREP_Location*
SCOREP_Location_AcquirePerProcessMetricsLocation( uint64_t* timestamp );

void
SCOREP_Location_ReleasePerProcessMetricsLocation( void );

/**
 *
 *
 * @return
 */
SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation( void );

/**
 *  Returns the local id of the location (ie. thread id).
 *
 * @return
 */
uint32_t
SCOREP_Location_GetId( SCOREP_Location* locationData );


/**
 *  Returns the type of the location.
 *
 * @return
 */
SCOREP_LocationType
SCOREP_Location_GetType( SCOREP_Location* locationData );


void*
SCOREP_Location_GetSubsystemData( SCOREP_Location* locationData,
                                  size_t           subsystem_id );


void
SCOREP_Location_SetSubsystemData( SCOREP_Location* locationData,
                                  size_t           subsystem_id,
                                  void*            subsystem_data );


void
SCOREP_Location_ForAll( bool ( * cb )( SCOREP_Location*,
                                       void* ),
                        void*    data );


SCOREP_LocationHandle
SCOREP_Location_GetLocationHandle( SCOREP_Location* locationData );

/**
 *  Calculate 64bit global ID from 32bit rank and thread/locationId
 *  via bit shift.
 *
 * @return global id
 */
uint64_t
SCOREP_Location_CalculateGlobalId( int      rank,
                                   uint32_t thread );

/**
 *  Calculate 32bit local id from 64bit global ID
 *  via bit shift.
 *
 * @return local Id
 */
uint32_t
SCOREP_Location_CalculateLocalId( uint64_t globalId );

/**
 *  Calculate 32bit rank from 64bit global ID
 *  via bit shift.
 *
 * @return rank
 */
uint32_t
SCOREP_Location_CalculateRank( uint64_t globalId );

uint64_t
SCOREP_Location_GetGlobalId( SCOREP_Location* locationData );

uint64_t
SCOREP_Location_GetLastTimestamp( SCOREP_Location* locationData );

void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  uint64_t         timestamp );

const char*
SCOREP_Location_GetName( SCOREP_Location* locationData );

#endif /* SCOREP_LOCATION_H */
