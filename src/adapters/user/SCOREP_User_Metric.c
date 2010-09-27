/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
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
#include "SCOREP_User_Functions.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_DefinitionLocking.h"
#include "SCOREP_Events.h"
#include "scorep_utility/SCOREP_Error.h"
#include "SCOREP_User_Init.h"
#include "SCOREP_Types.h"
#include "scorep_utility/SCOREP_Utils.h"

SCOREP_CounterGroupHandle SCOREP_User_DefaultMetricGroup = SCOREP_INVALID_COUNTER_GROUP;

void
scorep_user_init_metric()
{
    SCOREP_User_DefaultMetricGroup = SCOREP_DefineCounterGroup( "default" );
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
    SCOREP_LockCounterGroupDefinition();

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
    SCOREP_UnlockCounterGroupDefinition();
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
    SCOREP_LockCounterDefinition();

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
    SCOREP_UnlockCounterDefinition();
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
