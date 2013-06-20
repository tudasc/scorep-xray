/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 *
 *  @file       scorep_metric_management_mockup.c
 *
 *  @author     Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 *  @brief On systems without support for any metric source this module
 *         provides a basic implementation of metric handling.
 */

#include <config.h>

#include <SCOREP_Metric_Management.h>
#include "SCOREP_Metric.h"


void
SCOREP_Metric_InitializeMpp( void )
{
}

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

uint32_t
SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics( void )
{
    return 0;
}

SCOREP_MetricHandle
SCOREP_Metric_GetStrictlySynchronousMetricHandle( uint32_t index )
{
    return SCOREP_INVALID_METRIC;
}

void
SCOREP_Metric_WriteToTrace( SCOREP_Location* location,
                            uint64_t         timestamp )
{
}

void
SCOREP_Metric_WriteToProfile( SCOREP_Location* location )
{
}


/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Dummy implementation of the metric service
 */
const SCOREP_Subsystem SCOREP_Subsystem_MetricService =
{
    .subsystem_name              = "METRIC",
    .subsystem_register          = NULL,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL
};
