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


/**
 * @file       SCOREP_Thread_Types.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <SCOREP_Metric.h>


typedef struct SCOREP_Thread_LocationData SCOREP_Thread_LocationData;


/**
 *
 *
 * @return
 */
SCOREP_Thread_LocationData*
SCOREP_Thread_GetLocationData();


/**
 *  Returns the local id of the location (ie. thread id).
 *
 * @return
 */
uint32_t
SCOREP_Thread_GetLocationId( SCOREP_Thread_LocationData* locationData );


struct SCOREP_Metric_LocationData;
struct SCOREP_Metric_LocationData*
SCOREP_Thread_GetMetricLocationData( SCOREP_Thread_LocationData* locationData );


void
SCOREP_Thread_ForAllLocations( void ( * cb )( SCOREP_Thread_LocationData*,
                                              void* ),
                               void* data );


#endif /* SCOREP_THREAD_TYPES_H */
