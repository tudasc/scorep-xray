/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>

#include <stdlib.h>

#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Paradigms.h>

#include "scorep_type_utils.h"

/* Array of paradigm objects */
static SCOREP_Paradigm* registered_paradigms[ SCOREP_INVALID_PARADIGM_TYPE ];

void
SCOREP_Paradigms_Initialize( void )
{
    /* Nothing to be done here. But keep this function to avoid link errors. */
}

void
SCOREP_Paradigms_RegisterParallelParadigm( SCOREP_ParadigmType  paradigm,
                                           SCOREP_ParadigmClass paradigmClass,
                                           const char*          name,
                                           SCOREP_ParadigmFlags paradigmFlags )
{
    UTILS_BUG_ON( paradigm >= SCOREP_INVALID_PARADIGM_TYPE || paradigm < 0,
                  "Invalid paradigm.: %u", paradigm );

    UTILS_BUG_ON( registered_paradigms[ paradigm ] != NULL,
                  "Registering the same paradigm twice: %s",
                  SCOREP_Paradigms_GetParadigmName( paradigm ) );

    registered_paradigms[ paradigm ] =
        SCOREP_Definitions_NewParadigm( paradigm,
                                        paradigmClass,
                                        name,
                                        paradigmFlags );
}

void
SCOREP_Paradigms_SetStringProperty( SCOREP_ParadigmType     paradigm,
                                    SCOREP_ParadigmProperty paradigmProperty,
                                    const char*             propertyValue )
{
    UTILS_BUG_ON( paradigm >= SCOREP_INVALID_PARADIGM_TYPE || paradigm < 0,
                  "Invalid paradigm.: %u", paradigm );

    UTILS_BUG_ON( registered_paradigms[ paradigm ] == NULL,
                  "Unregistered paradigm.",
                  paradigm );

    SCOREP_Definitions_ParadigmSetProperty(
        registered_paradigms[ paradigm ],
        paradigmProperty,
        SCOREP_Definitions_NewString( propertyValue ) );
}


const char*
SCOREP_Paradigms_GetParadigmName( SCOREP_ParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm >= SCOREP_INVALID_PARADIGM_TYPE || paradigm < 0,
                  "Invalid paradigm.: %u", paradigm );

    UTILS_BUG_ON( registered_paradigms[ paradigm ] == NULL,
                  "Unregistered paradigm.",
                  paradigm );

    return registered_paradigms[ paradigm ]->name;
}


SCOREP_ParadigmClass
SCOREP_Paradigms_GetParadigmClass( SCOREP_ParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm >= SCOREP_INVALID_PARADIGM_TYPE || paradigm < 0,
                  "Invalid paradigm.: %u", paradigm );

    UTILS_BUG_ON( registered_paradigms[ paradigm ] == NULL,
                  "Unregistered paradigm.",
                  paradigm );

    return registered_paradigms[ paradigm ]->paradigm_class;
}
