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
 *  @file       SCOREP_User_MetricF.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  user metrics for Fortran macros.
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
#include "SCOREP_Fortran_Wrapper.h"

#define SCOREP_User_InitMetricGroupF_L scorep_user_initmetricgroupf
#define SCOREP_User_InitMetricGroupF_U SCOREP_USER_INITMETRICGROUPF
#define SCOREP_User_InitMetricF_L      scorep_user_initmetricf
#define SCOREP_User_InitMetricF_U      SCOREP_USER_INITMETRICF
#define SCOREP_User_MetricInt64F_L     scorep_user_metricint64f
#define SCOREP_User_MetricInt64F_U     SCOREP_USER_METRICINT64F
#define SCOREP_User_MetricDoubleF_L    scorep_user_metricdoublef
#define SCOREP_User_MetricDoubleF_U    SCOREP_USER_METRICDOUBLEF

extern SCOREP_CounterGroupHandle SCOREP_User_DefaultMetricGroup;

void
FSUB( SCOREP_User_InitMetricGroupF )( SCOREP_Fortran_MetricGroup* groupHandle,
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
    SCOREP_LockCounterGroupDefinition();

    /* Test if handle is already initialized */
    if ( *groupHandle != SCOREP_FORTRAN_INVALID_GROUP )
    {
        *groupHandle = SCOREP_C2F_COUNTER_GROUP( SCOREP_DefineCounterGroup( name ) );
    }
    else
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_USER | SCOREP_WARNING,
                             "Reinitializtaion of user metric group not possible\n" );
    }

    /* Clean up */
    SCOREP_UnlockCounterGroupDefinition();
    free( name );
}

void
FSUB( SCOREP_User_InitMetricF )
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

    /* Check if group handle is valid */
    if ( *groupF == SCOREP_FORTRAN_INVALID_GROUP )
    {
        SCOREP_ERROR( SCOREP_ERROR_USER_INVALID_MGROUP, "" );
    }
    else
    {
        /* Lock metric definition */
        SCOREP_LockCounterDefinition();

        /* Check if metric handle is already initialized */
        if ( *metricHandle != SCOREP_FORTRAN_INVALID_METRIC )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_USER | SCOREP_WARNING,
                                 "Reinitializtaion of user metric not possible\n" );
        }
        else
        {
            /* Define user metric */
            *metricHandle = SCOREP_C2F_COUNTER( SCOREP_DefineCounter( name, *metricType, group, unit ) );
        }
        /* Unlock metric definition */
        SCOREP_UnlockCounterDefinition();
    }

    /* Clean up */
    free( name );
    free( unit );
}

void
FSUB( SCOREP_User_MetricInt64F )( SCOREP_Fortran_MetricHandle* metric,
                                  int64_t*                     value )
{
    SCOREP_TriggerCounterInt64( SCOREP_F2C_COUNTER( *metric ), *value );
}

void
FSUB( SCOREP_User_MetricDoubleF )( SCOREP_Fortran_MetricHandle* metric,
                                   double*                      value )
{
    SCOREP_TriggerCounterDouble( SCOREP_F2C_COUNTER( *metric ), *value );
}
