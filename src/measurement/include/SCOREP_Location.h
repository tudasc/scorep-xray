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

#ifndef SCOREP_LOCATION_H
#define SCOREP_LOCATION_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "SCOREP_Types.h"
#include "SCOREP_Definitions.h"

/**
 * @file       SCOREP_Location.h
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
 *
 *
 * @return
 */
SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation();

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
                        void* data );


SCOREP_LocationHandle
SCOREP_Location_GetLocationHandle( SCOREP_Location* locationData );

uint64_t
SCOREP_Location_GetGlobalId( SCOREP_Location* locationData );

uint64_t
SCOREP_Location_GetLastTimestamp( SCOREP_Location* locationData );

void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  int64_t          timestamp );

#endif /* SCOREP_LOCATION_H */
