/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
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

#include "scorep_types.h"
#include "scorep_status.h"

#include <tracing/SCOREP_Tracing.h>

#include "scorep_properties.h"
#include <SCOREP_Memory.h>

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
    // SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_PTHREAD_LOCATION_REUSED
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = false
    }
};


void
scorep_properties_initialize( void )
{
    /* define properties */
    for ( int i = 0; i < SCOREP_PROPERTY_MAX; i++ )
    {
        scorep_properties[ i ].handle = SCOREP_Definitions_NewProperty(
            i,
            scorep_properties[ i ].condition,
            scorep_properties[ i ].initialValue );
    }
}

void
scorep_properties_write( void )
{
    if ( SCOREP_Status_GetRank() != 0 )
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
    SCOREP_PropertyDef* property_definition = SCOREP_LOCAL_HANDLE_DEREF(
        scorep_properties[ property ].handle,
        Property );
    property_definition->invalidated = true;
}
