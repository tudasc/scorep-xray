/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file contains the implementation of functions for selective recording.
 */

#include <config.h>

#include "scorep_selective_region.h"
#include <UTILS_Error.h>
#include <SCOREP_Memory.h>
#include <SCOREP_RuntimeManagement.h>

#include <assert.h>

SCOREP_User_RegionHandle
scorep_user_create_region( const char* name )
{
    SCOREP_User_Region* region = ( SCOREP_User_Region* )
                                 SCOREP_Memory_AllocForMisc( sizeof( SCOREP_User_Region ) );
    if ( region == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Unable to allocate memory for user region '%s'", name );
        return NULL;
    }

    region->handle                 = SCOREP_INVALID_REGION;
    region->selection              = scorep_selective_get_region( name );
    region->current_interval_index = 0;
    region->current_interval.first = 0;
    region->current_interval.last  = 0;
    region->enters                 = 0;
    region->exits                  = 0;
    region->has_enabled            = false;

    if ( region->selection != NULL )
    {
        region->current_interval = *( scorep_selected_interval* )
                                   SCOREP_Vector_At( region->selection->intervals, 0 );
    }

    return region;
}

bool
scorep_selective_check_enter( SCOREP_User_RegionHandle region )
{
    assert( region != SCOREP_USER_INVALID_REGION );

    /* Check whether this region has an selection entry */
    if ( region->selection == NULL )
    {
        return SCOREP_RecordingEnabled();
    }

    /* If this region has enabled recording before */
    if ( region->has_enabled )
    {
        region->enters++;
        region->exits++;
        return true;
    }
    /* This region has not enabled recording */

    /* But a parent region has already enabled recording */
    if ( SCOREP_RecordingEnabled() )
    {
        region->enters++;
        return true;
    }
    /* Recording is off */

    /* and a traced region is entered */
    if ( ( region->enters >= region->current_interval.first ) &&
         ( region->enters <= region->current_interval.last ) )
    {
        SCOREP_EnableRecording();
        /* The measurement system may refuse to switch */
        if ( SCOREP_RecordingEnabled() )
        {
            region->enters++;
            region->exits       = 1;
            region->has_enabled = true;
            return true;
        }
        return false;
    }

    /* It is not a traced instance */
    region->enters++;
    return false;
}

bool
scorep_selective_check_exit( SCOREP_User_RegionHandle region )
{
    assert( region != SCOREP_USER_INVALID_REGION );

    /* If this region has enabled recording */
    if ( region->has_enabled )
    {
        region->exits--;
        /* Check whether recording must be disabled */
        if ( region->exits <= 0 )
        {
            SCOREP_DisableRecording();
            region->has_enabled = false;
            /* Check whether we need to find the next interval */
            while ( region->current_interval.last < region->enters )
            {
                if ( region->current_interval_index < SCOREP_Vector_Size( region->selection->intervals ) - 1 )
                {
                    region->current_interval_index++;
                    region->current_interval = *( scorep_selected_interval* )
                                               SCOREP_Vector_At( region->selection->intervals, region->current_interval_index );
                }
                else
                {
                    /* No more instances are selected */
                    region->selection = NULL;
                    return true;
                }
            }
        }
        return true;
    }

    return SCOREP_RecordingEnabled();
}
