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

#ifndef SCOREP_METRIC_MANAGEMENT_H
#define SCOREP_METRIC_MANAGEMENT_H

/**
 *  @status     alpha
 *  @file       SCOREP_Metric_Management.h
 *  @author     Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 *  @maintainer Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 */

#include <UTILS_Error.h>
#include <SCOREP_Location.h>

/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

/** @brief  Get recent values of all metrics.
 *
 *  @return Returns pointer to value array filled with recent metric values,
 *          or NULL if we don't have metrics to read from.
 */
uint64_t*
SCOREP_Metric_Read( SCOREP_Location* locationData );

/** @brief  Reinitialize metric management. This functionality is used by
 *          Score-P Online Access to change recorded metrics between
 *          separate phases of program execution.
 */
SCOREP_Error_Code
SCOREP_Metric_Reinitialize();

/** @brief  Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSamplingSet();

#endif /* SCOREP_METRIC_MANAGEMENT_H */
