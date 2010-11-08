/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
#include <SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Mutex.h>

/**
   Contains the handle for the default metric group.
 */
SCOREP_CounterGroupHandle SCOREP_User_DefaultMetricGroup = SCOREP_INVALID_COUNTER_GROUP;

/**
   Mutex to avoid parallel assignments to the same user metrics group.
 */
SCOREP_Mutex scorep_user_metric_group_mutex;

/**
   Mutex to avoid parallel assignments to the same user metric.
 */
SCOREP_Mutex scorep_user_metric_mutex;


void
scorep_user_init_metric()
{
    SCOREP_MutexCreate( &scorep_user_metric_group_mutex );
    SCOREP_MutexCreate( &scorep_user_metric_mutex );
    SCOREP_User_DefaultMetricGroup = SCOREP_DefineCounterGroup( "default" );
}

void
scorep_user_final_metric()
{
    SCOREP_MutexDestroy( &scorep_user_metric_mutex );
    SCOREP_MutexDestroy( &scorep_user_metric_group_mutex );
}


void
SCOREP_User_InitMetricGroup
(
    SCOREP_CounterGroupHandle* groupHandle,
    const char*                name
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Lock metric group definition */
    SCOREP_MutexLock( scorep_user_metric_group_mutex );

    /* Test if handle is already initialized */
    if ( *groupHandle != SCOREP_INVALID_COUNTER_GROUP )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_USER | SCOREP_WARNING,
                             "Reinitializtaion of user metric group not possible\n" );
    }
    else
    {
        /* Define metric group handle */
        *groupHandle = SCOREP_DefineCounterGroup( name );
    }

    /* Unlock metric group definition */
    SCOREP_MutexUnlock( scorep_user_metric_group_mutex );
}

void
SCOREP_User_InitMetric
(
    SCOREP_CounterHandle*           metricHandle,
    const char*                     name,
    const char*                     unit,
    const SCOREP_User_MetricType    metricType,
    const int8_t                    context,
    const SCOREP_CounterGroupHandle group
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Check if group handle is valid */
    if ( group == SCOREP_INVALID_COUNTER_GROUP )
    {
        SCOREP_ERROR( SCOREP_ERROR_USER_INVALID_MGROUP, "" );
        return;
    }

    /* Lock metric definition */
    SCOREP_MutexLock( scorep_user_metric_mutex );

    /* Check if metric handle is already initialized */
    if ( *metricHandle != SCOREP_INVALID_COUNTER )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_USER | SCOREP_WARNING,
                             "Reinitializtaion of user metric not possible\n" );
    }
    else
    {
        /* Define user metric */
        *metricHandle = SCOREP_DefineCounter( name, metricType, group, unit );
    }

    /* Unlock metric definition */
    SCOREP_MutexUnlock( scorep_user_metric_mutex );
}

void
SCOREP_User_TriggerMetricInt64
(
    SCOREP_CounterHandle metricHandle,
    int64_t              value
)
{
    SCOREP_TriggerCounterInt64( metricHandle, value );
}

void
SCOREP_User_TriggerMetricDouble
(
    SCOREP_CounterHandle metricHandle,
    double               value
)
{
    SCOREP_TriggerCounterDouble( metricHandle, value );
}
