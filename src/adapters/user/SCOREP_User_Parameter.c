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
 *  @file       SCOREP_User_Parameter.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  parameters.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Types.h>

#define SCOREP_PARAMETER_TO_USER( handle ) ( ( uint64_t )( handle ) )
#define SCOREP_PARAMETER_FROM_USER( handle ) ( ( SCOREP_ParameterHandle )( handle ) )

void
SCOREP_User_ParameterInt64
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    int64_t                      value
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    if ( handle == NULL )
    {
        return;
    }

    /* Initialize parameter handle if necassary */
    if ( *handle == SCOREP_USER_INVALID_PARAMETER )
    {
        *handle = SCOREP_PARAMETER_TO_USER( SCOREP_Definitions_NewParameter( name, SCOREP_PARAMETER_INT64 ) );
    }

    /* Trigger event */
    SCOREP_TriggerParameterInt64( SCOREP_PARAMETER_FROM_USER( *handle ), value );
}

void
SCOREP_User_ParameterUint64
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    uint64_t                     value
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    if ( handle == NULL )
    {
        return;
    }

    /* Initialize parameter handle if necassary */
    if ( *handle == SCOREP_USER_INVALID_PARAMETER )
    {
        *handle = SCOREP_PARAMETER_TO_USER( SCOREP_Definitions_NewParameter( name, SCOREP_PARAMETER_UINT64 ) );
    }

    /* Trigger event */
    SCOREP_TriggerParameterUint64( SCOREP_PARAMETER_FROM_USER( *handle ), value );
}


void
SCOREP_User_ParameterString
(
    SCOREP_User_ParameterHandle* handle,
    const char*                  name,
    char*                        value
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    if ( handle == NULL )
    {
        return;
    }

    /* Initialize parameter handle if necassary */
    if ( *handle == SCOREP_USER_INVALID_PARAMETER )
    {
        *handle = SCOREP_PARAMETER_TO_USER( SCOREP_Definitions_NewParameter( name, SCOREP_PARAMETER_STRING ) );
    }

    /* Trigger event */
    SCOREP_TriggerParameterString( SCOREP_PARAMETER_FROM_USER( *handle ), value );
}
