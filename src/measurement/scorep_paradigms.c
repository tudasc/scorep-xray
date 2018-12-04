/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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

#include "scorep_types.h"

/* The compile time static number of known parallel paradigms */
#define N_PARALLEL_PARADIGMS \
    ( SCOREP_INVALID_PARADIGM_TYPE - SCOREP_PARADIGM_MPI )

/* Array of paradigm objects */
static SCOREP_Paradigm* registered_paradigms[ N_PARALLEL_PARADIGMS ];

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
    UTILS_BUG_ON( paradigm < SCOREP_PARADIGM_MPI,
                  "Non-parallel paradigm passed to register function: %s",
                  scorep_paradigm_type_to_string( paradigm ) );

    size_t paradigm_index = paradigm - SCOREP_PARADIGM_MPI;

    UTILS_BUG_ON( paradigm_index >= N_PARALLEL_PARADIGMS,
                  "Dynamic paradigms not yet supported.: %u", paradigm );

    UTILS_BUG_ON( registered_paradigms[ paradigm_index ],
                  "Registering the same paradigm twice: %s",
                  SCOREP_Paradigms_GetParadigmName( paradigm ) );

    registered_paradigms[ paradigm_index ] =
        SCOREP_Definitions_NewParadigm( paradigm,
                                        paradigmClass,
                                        name,
                                        paradigmFlags );
}

static size_t
get_paradigm_index( SCOREP_ParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm < SCOREP_PARADIGM_MPI,
                  "Invalid parallel paradigm: %s",
                  scorep_paradigm_type_to_string( paradigm ) );

    size_t paradigm_index = paradigm - SCOREP_PARADIGM_MPI;

    /* Parallel paradigms need to be registered first, if not its a BUG */
    UTILS_BUG_ON( paradigm_index >= N_PARALLEL_PARADIGMS
                  || registered_paradigms[ paradigm_index ] == NULL,
                  "Unregistered parallel paradigm: %u",
                  paradigm );

    return paradigm_index;
}


void
SCOREP_Paradigms_SetStringProperty( SCOREP_ParadigmType     paradigm,
                                    SCOREP_ParadigmProperty paradigmProperty,
                                    const char*             propertyValue )
{
    size_t paradigm_index = get_paradigm_index( paradigm );

    SCOREP_Definitions_ParadigmSetProperty(
        registered_paradigms[ paradigm_index ],
        paradigmProperty,
        SCOREP_Definitions_NewString( propertyValue ) );
}


const char*
SCOREP_Paradigms_GetParadigmName( SCOREP_ParadigmType paradigm )
{
    /* For non-parallel paradigms, just return the hard coded one */
    if ( paradigm < SCOREP_PARADIGM_MPI )
    {
        return scorep_paradigm_type_to_string( paradigm );
    }

    size_t paradigm_index = get_paradigm_index( paradigm );

    return registered_paradigms[ paradigm_index ]->name;
}


SCOREP_ParadigmClass
SCOREP_Paradigms_GetParadigmClass( SCOREP_ParadigmType paradigm )
{
    /* For non-parallel paradigms, just return INVALID */
    if ( paradigm < SCOREP_PARADIGM_MPI )
    {
        return SCOREP_INVALID_PARADIGM_CLASS;
    }

    size_t paradigm_index = get_paradigm_index( paradigm );

    return registered_paradigms[ paradigm_index ]->paradigm_class;
}
