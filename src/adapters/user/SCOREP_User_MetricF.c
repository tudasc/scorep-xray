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
 *  @file       SCOREP_User_MetricF.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  user metrics for Fortran macros.
 */

#include <config.h>
#include <SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Events.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Fortran_Wrapper.h>

#define SCOREP_F_InitMetricGroup_L scorep_f_initmetricgroup
#define SCOREP_F_InitMetricGroup_U SCOREP_F_INITMETRICGROUP
#define SCOREP_F_InitMetric_L      scorep_f_initmetric
#define SCOREP_F_InitMetric_U      SCOREP_F_INITMETRIC
#define SCOREP_F_MetricInt64_L     scorep_f_metricint64
#define SCOREP_F_MetricInt64_U     SCOREP_F_METRICINT64
#define SCOREP_F_MetricDouble_L    scorep_f_metricdouble
#define SCOREP_F_MetricDouble_U    SCOREP_F_METRICDOUBLE

extern SCOREP_CounterGroupHandle SCOREP_User_DefaultMetricGroup;
extern SCOREP_Mutex              scorep_user_metric_group_mutex;
extern SCOREP_Mutex              scorep_user_metric_mutex;

void
FSUB( SCOREP_F_InitMetricGroup )( SCOREP_Fortran_MetricGroup* groupHandle,
                                  char*                       nameF,
                                  int                         nameLen )
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Convert name to C-String */
    char* name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, nameF, nameLen );
    name[ nameLen ] = '\0';

    /* Lock metric group definition */
    SCOREP_MutexLock( scorep_user_metric_group_mutex );

    /* Make sure handle is already initialized */
    *groupHandle = SCOREP_C2F_COUNTER_GROUP( SCOREP_DefineCounterGroup( name ) );

    /* Clean up */
    SCOREP_MutexUnlock( scorep_user_metric_group_mutex );
    free( name );
}

void
FSUB( SCOREP_F_InitMetric )
(
    SCOREP_Fortran_MetricHandle* metricHandle,
    char*                        nameF,
    char*                        unitF,
    int*                         metricType,
    int*                         context,
    SCOREP_Fortran_MetricGroup*  groupF,
    int                          nameLen,
    int                          unitLen
)
{
    char*                     name;
    char*                     unit;
    SCOREP_CounterGroupHandle group;

    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Convert name to C-String */
    name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, nameF, nameLen );
    name[ nameLen ] = '\0';

    /* Convert unit to C-String */
    unit = ( char* )malloc( ( unitLen + 1 ) * sizeof( char ) );
    strncpy( unit, unitF, unitLen );
    unit[ unitLen ] = '\0';

    /* Check for default group */
    group = ( *groupF == SCOREP_FORTRAN_DEFAULT_GROUP ?
              SCOREP_User_DefaultMetricGroup : SCOREP_F2C_COUNTER_GROUP( *groupF ) );

    /* Lock metric definition */
    SCOREP_MutexLock( scorep_user_metric_mutex );

    /* Make sure handle is initialized */
    *metricHandle = SCOREP_C2F_COUNTER( SCOREP_DefineCounter( name, *metricType, group, unit ) );

    /* Unlock metric definition */
    SCOREP_MutexUnlock( scorep_user_metric_mutex );

    /* Clean up */
    free( name );
    free( unit );
}

void
FSUB( SCOREP_F_MetricInt64 )( SCOREP_Fortran_MetricHandle* metric,
                              int64_t*                     value )
{
    SCOREP_TriggerCounterInt64( SCOREP_F2C_COUNTER( *metric ), *value );
}

void
FSUB( SCOREP_F_MetricDouble )( SCOREP_Fortran_MetricHandle* metric,
                               double*                      value )
{
    SCOREP_TriggerCounterDouble( SCOREP_F2C_COUNTER( *metric ), *value );
}
