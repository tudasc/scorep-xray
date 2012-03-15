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

#ifndef SCOREP_THREAD_TYPES_H
#define SCOREP_THREAD_TYPES_H


#include <stddef.h>
#include <stdint.h>

#include "SCOREP_Types.h"

/**
 * @file       SCOREP_Thread_Types.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
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
SCOREP_CreateLocation( SCOREP_Location*    parent,
                       SCOREP_LocationType type,
                       const char*         name );


/**
 *
 *
 * @return
 */
SCOREP_Location*
SCOREP_Thread_GetLocationData();


/**
 *  Returns the local id of the location (ie. thread id).
 *
 * @return
 */
uint32_t
SCOREP_Thread_GetLocationId( SCOREP_Location* locationData );


/**
 *  Returns the type of the location.
 *
 * @return
 */
SCOREP_LocationType
SCOREP_Thread_GetLocationType( SCOREP_Location* locationData );


void*
SCOREP_Thread_GetSubsystemLocationData( SCOREP_Location* locationData,
                                        size_t           subsystem_id );


void
SCOREP_Thread_SetSubsystemLocationData( SCOREP_Location* locationData,
                                        size_t           subsystem_id,
                                        void*            subsystem_data );


void
SCOREP_Thread_ForAllLocations( void ( * cb )( SCOREP_Location*,
                                              void* ),
                               void* data );


#endif /* SCOREP_THREAD_TYPES_H */
