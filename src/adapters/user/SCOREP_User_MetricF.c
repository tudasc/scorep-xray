/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014-2015, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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
 *  This file containes the implementation of user adapter functions concerning
 *  user metrics for Fortran macros.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Types.h>
#include <SCOREP_Fortran_Wrapper.h>
#include <SCOREP_RuntimeManagement.h>

#include <stdlib.h>
#include <string.h>

#define SCOREP_F_InitMetric_L      scorep_f_initmetric
#define SCOREP_F_InitMetric_U      SCOREP_F_INITMETRIC
#define SCOREP_F_MetricInt64_L     scorep_f_metricint64
#define SCOREP_F_MetricInt64_U     SCOREP_F_METRICINT64
#define SCOREP_F_MetricUint64_L    scorep_f_metricuint64
#define SCOREP_F_MetricUint64_U    SCOREP_F_METRICUINT64
#define SCOREP_F_MetricDouble_L    scorep_f_metricdouble
#define SCOREP_F_MetricDouble_U    SCOREP_F_METRICDOUBLE

void
FSUB( SCOREP_F_InitMetric )( SCOREP_Fortran_MetricHandle* metricHandle,
                             char*                        nameF,
                             char*                        unitF,
                             int*                         metricType,
                             int*                         context,
                             scorep_fortran_charlen_t     nameLen,
                             scorep_fortran_charlen_t     unitLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        char* name;
        char* unit;

        /* Convert name to C-String */
        name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
        strncpy( name, nameF, nameLen );
        name[ nameLen ] = '\0';

        /* Convert unit to C-String */
        unit = ( char* )malloc( ( unitLen + 1 ) * sizeof( char ) );
        strncpy( unit, unitF, unitLen );
        unit[ unitLen ] = '\0';

        /* Make sure handle is initialized */
        SCOREP_SamplingSetHandle samplingSetHandle = SCOREP_INVALID_SAMPLING_SET;
        SCOREP_User_InitMetric( &samplingSetHandle, name, unit, *metricType, *context );
        *metricHandle = SCOREP_C2F_COUNTER( samplingSetHandle );

        /* Clean up */
        free( name );
        free( unit );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_MetricInt64 )( SCOREP_Fortran_MetricHandle* metric,
                              int64_t*                     value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterInt64( SCOREP_F2C_COUNTER( *metric ), *value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_MetricUint64 )( SCOREP_Fortran_MetricHandle* metric,
                               uint64_t*                    value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterUint64( SCOREP_F2C_COUNTER( *metric ), *value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_MetricDouble )( SCOREP_Fortran_MetricHandle* metric,
                               double*                      value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_TriggerCounterDouble( SCOREP_F2C_COUNTER( *metric ), *value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
