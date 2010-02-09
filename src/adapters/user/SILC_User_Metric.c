/*
 * This file is part of the SILC project (http://www.silc.de)
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

/** @file SILC_User_Metric.c
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file containes the implementation of user adapter functions concerning
    user metrics.
 */

#include "SILC_User_Functions.h"
#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_Error.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Utils.h"

SILC_CounterGroupHandle SILC_User_DefaultMetricGroup = SILC_INVALID_COUNTER_GROUP;

void
silc_user_init_metric()
{
    SILC_User_DefaultMetricGroup = SILC_DefineCounterGroup( "default" );
}


void
SILC_User_InitMetricGroup
(
    SILC_CounterGroupHandle* groupHandle,
    const char*              name
)
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Test if handle is already initialized */
    if ( *groupHandle != SILC_INVALID_COUNTER_GROUP )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_USER | SILC_WARNING,
                           "Reinitializtaion of user metric group not possible\n" );
        return;
    }

    /* Define metric group handle */
    *groupHandle = SILC_DefineCounterGroup( name );
}

void
SILC_User_InitMetric
(
    SILC_CounterHandle*           metricHandle,
    const char*                   name,
    const char*                   unit,
    const SILC_User_MetricType    metricType,
    const int8_t                  context,
    const SILC_CounterGroupHandle group
)
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Check if group handle is valid */
    if ( group == SILC_INVALID_COUNTER_GROUP )
    {
        SILC_ERROR( SILC_ERROR_USER_INVALID_MGROUP, "" );
        return;
    }

    /* Check if metric handle is already initialized */
    if ( *metricHandle != SILC_INVALID_COUNTER )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_USER | SILC_WARNING,
                           "Reinitializtaion of user metric not possible\n" );
        return;
    }

    /* Define user metric */
    *metricHandle = SILC_DefineCounter( name, metricType, group, unit );
}

void
SILC_User_TriggerMetricInt64
(
    SILC_CounterHandle metricHandle,
    int64_t            value
)
{
    SILC_TriggerCounterInt64( metricHandle, value );
}

void
SILC_User_TriggerMetricDouble
(
    SILC_CounterHandle metricHandle,
    double             value
)
{
    SILC_TriggerCounterDouble( metricHandle, value );
}
