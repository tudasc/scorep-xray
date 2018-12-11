/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013-2015, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
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
 *  parameters for Fortran.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Types.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Fortran_Wrapper.h>

#include <string.h>
#include <stdlib.h>

#define SCOREP_F_ParameterInt64_U SCOREP_F_PARAMETERINT64
#define SCOREP_F_ParameterUint64_U SCOREP_F_PARAMETERUINT64
#define SCOREP_F_ParameterString_U SCOREP_F_PARAMETERSTRING

#define SCOREP_F_ParameterInt64_L scorep_f_parameterint64
#define SCOREP_F_ParameterUint64_L scorep_f_parameteruint64
#define SCOREP_F_ParameterString_L scorep_f_parameterstring


void
FSUB( SCOREP_F_ParameterInt64 )( SCOREP_Fortran_Parameter* handle,
                                 const char*               name,
                                 int64_t*                  value,
                                 scorep_fortran_charlen_t  name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        char* c_name = NULL;

#ifdef SCOREP_COMPILER_PGI
        /* The PGI Fortran interface provides no handle initialization */
        *handle = SCOREP_USER_INVALID_PARAMETER;
#endif

        if ( *handle == SCOREP_USER_INVALID_PARAMETER )
        {
            c_name = malloc( name_len + 1 );
            strncpy( c_name, name, name_len );
            c_name[ name_len ] = '\0';
        }

        SCOREP_User_ParameterInt64( ( SCOREP_User_ParameterHandle* )handle, c_name, *value );

        free( c_name );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_ParameterUint64 )( SCOREP_Fortran_Parameter* handle,
                                  const char*               name,
                                  uint64_t*                 value,
                                  scorep_fortran_charlen_t  name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        char* c_name = NULL;

#ifdef SCOREP_COMPILER_PGI
        /* The PGI Fortran interface provides no handle initialization */
        *handle = SCOREP_USER_INVALID_PARAMETER;
#endif

        if ( *handle == SCOREP_USER_INVALID_PARAMETER )
        {
            c_name = malloc( name_len + 1 );
            strncpy( c_name, name, name_len );
            c_name[ name_len ] = '\0';
        }

        SCOREP_User_ParameterUint64( ( SCOREP_User_ParameterHandle* )handle, c_name, *value );

        free( c_name );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_ParameterString )( SCOREP_User_ParameterHandle* handle,
                                  const char*                  name,
                                  const char*                  value,
                                  scorep_fortran_charlen_t     name_len,
                                  scorep_fortran_charlen_t     value_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
#ifdef SCOREP_COMPILER_PGI
        /* The PGI Fortran interface provides no handle initialization */
        *handle = SCOREP_USER_INVALID_PARAMETER;
#endif

        char* c_name  = NULL;
        char* c_value = malloc( value_len + 1 );
        strncpy( c_value, value, value_len );
        c_value[ value_len ] = '\0';

        if ( *handle == SCOREP_USER_INVALID_PARAMETER )
        {
            c_name = malloc( name_len + 1 );
            strncpy( c_name, name, name_len );
            c_name[ name_len ] = '\0';
        }

        SCOREP_User_ParameterString( ( SCOREP_User_ParameterHandle* )handle, c_name, c_value );

        free( c_name );
        free( c_value );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
