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
 *  @file       scorep_metric_management_without_sources.c
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

/** @brief Called on deregistration of the metric service.
 */
static void
scorep_metric_deregister( void )
{
}

/** @brief Location specific finalization function for metric services.
 *
 *  @param location             Location data.
 *
 *  @param location Reference to location that will finalize its metric related data structures.
 */
static void
scorep_metric_finalize_location( SCOREP_Location* location )
{
}

/** @brief Service finalization.
 */
static void
scorep_metric_finalize_service( void )
{
}

/** @brief  Location specific initialization function for metric services.
 *
 *  @param location             Location data.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_initialize_location( SCOREP_Location* location )
{
    return SCOREP_SUCCESS;
}

/** @brief Called on initialization of the metric service.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_initialize_service( void )
{
    return SCOREP_SUCCESS;
}

/** @brief Registers configuration variables for the metric services.
 *
 *  @param subsystem_id         Identifier of metric subsystem.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_register( size_t subsystem_id )
{
    return SCOREP_SUCCESS;
}

/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Subsystem_MetricService =
{
    .subsystem_name              = "METRIC",
    .subsystem_register          = &scorep_metric_register,
    .subsystem_init              = &scorep_metric_initialize_service,
    .subsystem_init_location     = &scorep_metric_initialize_location,
    .subsystem_finalize_location = &scorep_metric_finalize_location,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &scorep_metric_finalize_service,
    .subsystem_deregister        = &scorep_metric_deregister
};
