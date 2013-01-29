/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @status     alpha
 * @file       src/measurement/scorep_properties.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Propertiy handling.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_DefinitionHandles.h>

#include "scorep_definition_structs.h"
#include "scorep_definitions.h"
#include "scorep_types.h"
#include "scorep_status.h"
#include "scorep_mpi.h"

#include <tracing/SCOREP_Tracing.h>

#include "scorep_properties.h"

static struct scorep_property
{
    SCOREP_PropertyHandle    handle;
    SCOREP_PropertyCondition condition;
    bool                     initialValue;
}
scorep_properties[ SCOREP_PROPERTY_MAX ] =
{
    // SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
};


void
scorep_properties_initialize( void )
{
    /* define properties */
    for ( int i = 0; i < SCOREP_PROPERTY_MAX; i++ )
    {
        scorep_properties[ i ].handle = SCOREP_DefineProperty(
            i,
            scorep_properties[ i ].condition,
            scorep_properties[ i ].initialValue );
    }
}

void
scorep_properties_write( void )
{
    if ( SCOREP_Mpi_GetRank() != 0 )
    {
        return;
    }
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_WriteProperties();
    }
}

void
SCOREP_InvalidateProperty( SCOREP_Property property )
{
    UTILS_ASSERT( property < SCOREP_PROPERTY_MAX );
    SCOREP_Property_Definition* property_definition = SCOREP_LOCAL_HANDLE_DEREF(
        scorep_properties[ property ].handle,
        Property );
    property_definition->invalidated = true;
}
