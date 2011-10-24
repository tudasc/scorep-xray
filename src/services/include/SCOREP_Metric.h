/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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


#ifndef SCOREP_METRIC_H_
#define SCOREP_METRIC_H_


/**
 * @ file      SCOREP_Metric.h
 * @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 * @brief Metric adapter initialization and finalization function definition.
 */

#include "SCOREP_Subsystem.h"


/** The adapter initialization and finalization function structure for the
 *  metric adapter. */
extern const SCOREP_Subsystem SCOREP_Metric_Service;

/** Thread local data related to metrics */
typedef struct SCOREP_Metric_LocationData SCOREP_Metric_LocationData;



/* *********************************************************************
 * Thread Events
 **********************************************************************/

/** Allocate and initialize a valid SCOREP_Metric_LocationData object. */
SCOREP_Metric_LocationData*
SCOREP_Metric_CreateLocationData();


/** Clean up @a metricLocationData at the end of a phase or at the end of the
 *  measurement.
 *
 *  @param metricLocationData The object to be deleted
 */
void
SCOREP_Metric_DeleteLocationData( SCOREP_Metric_LocationData* metricLocationData );

#endif /* SCOREP_METRIC_H_ */
