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
 * @file        scorep_profile_location.c
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status      alpha
 * @brief Contains implmentation for profile location data
 *
 */

#include <config.h>
#include <scorep_profile_location.h>
#include <scorep_thread.h>
#include <assert.h>
#include <SCOREP_Memory.h>

scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Profile_LocationData* location )
{
    return location->current_task_node;
}

void
scorep_profile_set_current_node( SCOREP_Profile_LocationData* location,
                                 scorep_profile_node*         node )
{
    location->current_task_node = node;
}

SCOREP_Profile_LocationData*
scorep_profile_create_location_data()
{
    /* Create location data structure.
     * The location data structure must not be deleted when the profile is reset
     * in a persicope phase. Thus the memory is not allocated from the profile
     * memory pages.
     */
    SCOREP_Profile_LocationData* location
        = SCOREP_Memory_AllocForMisc( sizeof( SCOREP_Profile_LocationData ) );

    /* Set default values. */
    location->current_implicit_node = NULL;
    location->root_node             = NULL;
    location->fork_node             = NULL;
    location->creation_node         = NULL;
    location->current_depth         = 0;
    location->implicit_depth        = 0;
    location->fork_depth            = 0;
    location->free_nodes            = NULL;
    location->free_int_metrics      = NULL;
    location->free_double_metrics   = NULL;
    location->current_task_node     = NULL;
    location->current_task          = SCOREP_PROFILE_IMPLICIT_TASK;

    /* Initialize locations task instance table */
    scorep_profile_task_initialize( location );

    return location;
}


void
scorep_profile_delete_location_data( SCOREP_Profile_LocationData* location )
{
    if ( location == NULL )
    {
        return;
    }

    /* Finalize locations task instance table */
    scorep_profile_task_finalize( location );
}
