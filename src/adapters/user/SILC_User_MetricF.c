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

/** @file       SILC_User_MetricF.c
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file containes the implementation of user adapter functions concerning
    user metrics for Fortran macros.
 */

#include "SILC_User_Functions.h"
#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_Error.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Utils.h"
#include "SILC_Fortran_Wrapper.h"

#define SILC_User_InitMetricGroupF_L silc_user_initmetricgroupf
#define SILC_User_InitMetricGroupF_U SILC_USER_INITMETRICGROUPF
#define SILC_User_InitMetricF_L      silc_user_initmetricf
#define SILC_User_InitMetricF_U      SILC_USER_INITMETRICF
#define SILC_User_MetricInt64F_L     silc_user_metricint64f
#define SILC_User_MetricInt64F_U     SILC_USER_METRICINT64F
#define SILC_User_MetricDoubleF_L    silc_user_metricdoublef
#define SILC_User_MetricDoubleF_U    SILC_USER_METRICDOUBLEF

extern SILC_CounterGroupHandle SILC_User_DefaultMetricGroup;

void
FSUB( SILC_User_InitMetricGroupF ) ( SILC_Fortran_MetricGroup * groupHandle,
                                     char* nameF,
                                     int nameLen )
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Convert name to C-String */
    char* name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, nameF, nameLen );
    name[ nameLen ] = '\0';

    /* Lock metric group definition */
    SILC_LockCounterGroupDefinition();

    /* Test if handle is already initialized */
    if ( *groupHandle != SILC_FORTRAN_INVALID_GROUP )
    {
        *groupHandle = SILC_DefineCounterGroup( name );
    }
    else
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_USER | SILC_WARNING,
                           "Reinitializtaion of user metric group not possible\n" );
    }

    /* Clean up */
    SILC_UnlockCounterGroupDefinition();
    free( name );
}

void
FSUB( SILC_User_InitMetricF )
(
    SILC_Fortran_MetricHandle *   metricHandle,
    char*  nameF,
    char*  unitF,
    int*   metricType,
    int*   context,
    SILC_Fortran_MetricGroup *   groupF,
    int nameLen,
    int unitLen
)
{
    char*                   name;
    char*                   unit;
    SILC_CounterGroupHandle group;

    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Convert name to C-String */
    name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, nameF, nameLen );
    name[ nameLen ] = '\0';

    /* Convert unit to C-String */
    unit = ( char* )malloc( ( unitLen + 1 ) * sizeof( char ) );
    strncpy( unit, unitF, unitLen );
    unit[ unitLen ] = '\0';

    /* Check for default group */
    group = ( *groupF == SILC_FORTRAN_DEFAULT_GROUP ?
              SILC_User_DefaultMetricGroup : ( SILC_CounterGroupHandle ) * groupF );

    /* Check if group handle is valid */
    if ( group == SILC_FORTRAN_INVALID_GROUP )
    {
        SILC_ERROR( SILC_ERROR_USER_INVALID_MGROUP, "" );
    }
    else
    {
        /* Lock metric definition */
        SILC_LockCounterDefinition();

        /* Check if metric handle is already initialized */
        if ( *metricHandle != SILC_FORTRAN_INVALID_METRIC )
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_USER | SILC_WARNING,
                               "Reinitializtaion of user metric not possible\n" );
        }
        else
        {
            /* Define user metric */
            *metricHandle = SILC_DefineCounter( name, *metricType, group, unit );
        }
        /* Unlock metric definition */
        SILC_UnlockCounterDefinition();
    }

    /* Clean up */
    free( name );
    free( unit );
}

void
FSUB( SILC_User_MetricInt64F ) ( SILC_Fortran_MetricHandle * metric, int64_t * value )
{
    SILC_TriggerCounterInt64( *metric, *value );
}

void
FSUB( SILC_User_MetricDoubleF ) ( SILC_Fortran_MetricHandle * metric, double* value )
{
    SILC_TriggerCounterDouble( *metric, *value );
}
