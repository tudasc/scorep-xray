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

/**
 *  @status     alpha
 *  @file       SCOREP_Metric_Management.h
 *  @author     Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 *  @maintainer Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 */

/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

/** @brief  Get recent values of all metrics.
 *
 *  @return Returns pointer to value array filled with recent metric values.
 */
uint64_t*
SCOREP_Metric_read( SCOREP_Thread_LocationData* locationData );

/** @brief  Reinitialize metric management. This functionality is used by
 *          Score-P Online Access to change recorded metrics between
 *          separate phases of program execution.
 */
SCOREP_Error_Code
SCOREP_Metric_reinitialize();

/** @brief  Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSamplingSet();

/* void
   SCOREP_Metric_read( struct SCOREP_Metric_EventSet* eventSet, uint64_t values[] );*/
