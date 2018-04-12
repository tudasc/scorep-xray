/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017-2018,
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
 *
 *  @file
 *
 *
 *  @brief On systems without support for any metric source this module
 *         provides a basic implementation of metric handling.
 */

#include <config.h>

#include <SCOREP_Metric_Management.h>
#include "SCOREP_Metric.h"


uint64_t*
SCOREP_Metric_Read( SCOREP_Location* location )
{
    return NULL;
}

SCOREP_ErrorCode
SCOREP_Metric_Reinitialize( void )
{
    return SCOREP_SUCCESS;
}

SCOREP_SamplingSetHandle
SCOREP_Metric_GetStrictlySynchronousSamplingSet( void )
{
    return SCOREP_INVALID_SAMPLING_SET;
}

SCOREP_MetricHandle
SCOREP_Metric_GetStrictlySynchronousMetricHandle( uint32_t index )
{
    return SCOREP_INVALID_METRIC;
}

uint32_t
SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics( void )
{
    return 0;
}

uint32_t
SCOREP_Metric_GetMaximalNumberOfSynchronousMetrics( SCOREP_Location* location )
{
    return 0;
}

void
SCOREP_Metric_WriteStrictlySynchronousMetrics( SCOREP_Location* location,
                                               uint64_t         timestamp,
                                               WriteMetricsCb   cb )
{
    return;
}

void
SCOREP_Metric_WriteSynchronousMetrics( SCOREP_Location* location,
                                       uint64_t         timestamp,
                                       WriteMetricsCb   cb )
{
    return;
}

void
SCOREP_Metric_WriteAsynchronousMetrics( SCOREP_Location* location,
                                        WriteMetricsCb   cb )
{
    return;
}


/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Dummy implementation of the metric service
 */
const SCOREP_Subsystem SCOREP_Subsystem_MetricService =
{
    .subsystem_name = "METRIC",
};
