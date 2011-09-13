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
 *  @file       SCOREP_User_ParameterF.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  parameters for Fortran.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Fortran_Wrapper.h>

#include <string.h>

#define SCOREP_F_ParameterInt64_U SCOREP_F_PARAMETERINT64
#define SCOREP_F_ParameterUint64_U SCOREP_F_PARAMETERUINT64
#define SCOREP_F_ParameterString_U SCOREP_F_PARAMETERSTRING

#define SCOREP_F_ParameterInt64_L scorep_f_parameterint64
#define SCOREP_F_ParameterUint64_L scorep_f_parameteruint64
#define SCOREP_F_ParameterString_L scorep_f_parameterstring


void
FSUB( SCOREP_F_ParameterInt64 )
(
    SCOREP_Fortran_Parameter* handle,
    const char*               name,
    int64_t*                  value,
    int                       name_len
)
{
    char* c_name = NULL;
    if ( *handle == SCOREP_INVALID_PARAMETER )
    {
        c_name = malloc( name_len + 1 );
        strncpy( c_name, name, name_len );
        c_name[ name_len ] = '\0';
    }

    SCOREP_User_ParameterInt64( ( SCOREP_User_ParameterHandle* )handle, c_name, *value );

    free( c_name );
}

void
FSUB( SCOREP_F_ParameterUint64 )
(
    SCOREP_Fortran_Parameter* handle,
    const char*               name,
    uint64_t*                 value,
    int                       name_len
)
{
    char* c_name = NULL;
    if ( *handle == SCOREP_INVALID_PARAMETER )
    {
        c_name = malloc( name_len + 1 );
        strncpy( c_name, name, name_len );
        c_name[ name_len ] = '\0';
    }

    SCOREP_User_ParameterUint64( ( SCOREP_User_ParameterHandle* )handle, c_name, *value );

    free( c_name );
}

void
FSUB( SCOREP_F_ParameterString )
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    char*                        value,
    int                          name_len,
    int                          value_len
)
{
    char* c_name  = NULL;
    char* c_value = malloc( value_len + 1 );
    strncpy( c_value, value, value_len );
    c_value[ value_len ] = '\0';

    if ( *handle == SCOREP_INVALID_PARAMETER )
    {
        c_name = malloc( name_len + 1 );
        strncpy( c_name, name, name_len );
        c_name[ name_len ] = '\0';
    }

    SCOREP_User_ParameterString( ( SCOREP_User_ParameterHandle* )handle, c_name, c_value );

    free( c_name );
    free( c_value );
}
