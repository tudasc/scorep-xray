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

/**
 *  @file       SCOREP_User_Metric.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  user metrics.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Mutex.h>

/**
   Mutex to avoid parallel assignments to the same user metric.
 */
SCOREP_Mutex scorep_user_metric_mutex;


void
scorep_user_init_metric()
{
    SCOREP_MutexCreate( &scorep_user_metric_mutex );
}

void
scorep_user_final_metric()
{
    SCOREP_MutexDestroy( &scorep_user_metric_mutex );
}


void
SCOREP_User_InitMetric
(
    SCOREP_SamplingSetHandle*    metricHandle,
    const char*                  name,
    const char*                  unit,
    const SCOREP_User_MetricType metricType,
    const int8_t                 context
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Lock metric definition */
    SCOREP_MutexLock( scorep_user_metric_mutex );

    /* Check if metric handle is already initialized */
    if ( *metricHandle != SCOREP_INVALID_SAMPLING_SET )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_USER | SCOREP_DEBUG_WARNING,
                             "Reinitializtaion of user metric not possible\n" );
    }
    else
    {
        SCOREP_MetricValueType value_type;
        switch ( metricType )
        {
            case SCOREP_USER_METRIC_TYPE_INT64:
                value_type = SCOREP_METRIC_VALUE_INT64;
                break;
            case SCOREP_USER_METRIC_TYPE_UINT64:
                value_type = SCOREP_METRIC_VALUE_UINT64;
                break;
            case SCOREP_USER_METRIC_TYPE_DOUBLE:
                value_type = SCOREP_METRIC_VALUE_DOUBLE;
                break;
            default:
                SCOREP_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                              "Invalid metric type given." );
                goto out;
        }

        /* Define user metric */
        SCOREP_MetricHandle metric
            = SCOREP_DefineMetric( name,
                                   "",
                                   SCOREP_METRIC_SOURCE_TYPE_USER,
                                   SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                   value_type,
                                   SCOREP_METRIC_BASE_DECIMAL,
                                   0,
                                   unit,
                                   SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

        *metricHandle
            = SCOREP_DefineSamplingSet( 1, &metric,
                                        SCOREP_METRIC_OCCURRENCE_ASYNCHRONOUS );
    }

out:
    /* Unlock metric definition */
    SCOREP_MutexUnlock( scorep_user_metric_mutex );
}

void
SCOREP_User_TriggerMetricInt64
(
    SCOREP_SamplingSetHandle metricHandle,
    int64_t                  value
)
{
    SCOREP_TriggerCounterInt64( metricHandle, value );
}

void
SCOREP_User_TriggerMetricUint64
(
    SCOREP_SamplingSetHandle metricHandle,
    uint64_t                 value
)
{
    SCOREP_TriggerCounterUint64( metricHandle, value );
}

void
SCOREP_User_TriggerMetricDouble
(
    SCOREP_SamplingSetHandle metricHandle,
    double                   value
)
{
    SCOREP_TriggerCounterDouble( metricHandle, value );
}
