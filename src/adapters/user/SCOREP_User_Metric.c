/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file contains the implementation of user adapter functions concerning
 *  user metrics.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Types.h>
#include <SCOREP_RuntimeManagement.h>
#include <UTILS_Error.h>
#include <UTILS_Debug.h>


void
SCOREP_User_InitMetric( SCOREP_SamplingSetHandle*    metricHandle,
                        const char*                  name,
                        const char*                  unit,
                        const SCOREP_User_MetricType metricType,
                        const int8_t                 context )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* Lock metric definition */
    SCOREP_MutexLock( scorep_user_metric_mutex );

    /* Check if metric handle is already initialized */
    if ( *metricHandle != SCOREP_INVALID_SAMPLING_SET )
    {
        UTILS_WARNING( "Reinitialization of user metric not possible" );
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
                UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                             "Invalid metric type given." );
                goto out;
        }

        /* Define user metric */
        SCOREP_MetricHandle metric
            = SCOREP_Definitions_NewMetric( name,
                                            "",
                                            SCOREP_METRIC_SOURCE_TYPE_USER,
                                            SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                            value_type,
                                            SCOREP_METRIC_BASE_DECIMAL,
                                            0,
                                            unit,
                                            SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                            SCOREP_INVALID_METRIC );

        *metricHandle
            = SCOREP_Definitions_NewSamplingSet( 1, &metric,
                                                 SCOREP_METRIC_OCCURRENCE_ASYNCHRONOUS,
                                                 SCOREP_SAMPLING_SET_CPU );
    }

out:
    /* Unlock metric definition */
    SCOREP_MutexUnlock( scorep_user_metric_mutex );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_TriggerMetricInt64( SCOREP_SamplingSetHandle metricHandle,
                                int64_t                  value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterInt64( metricHandle, value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_TriggerMetricUint64( SCOREP_SamplingSetHandle metricHandle,
                                 uint64_t                 value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterUint64( metricHandle, value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_TriggerMetricDouble( SCOREP_SamplingSetHandle metricHandle,
                                 double                   value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterDouble( metricHandle, value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
