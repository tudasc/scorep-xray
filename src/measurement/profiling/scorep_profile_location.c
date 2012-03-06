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

scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Location* thread )
{
    return SCOREP_Thread_GetProfileLocationData( thread )->current_task_node;
}

void
scorep_profile_set_current_node( SCOREP_Location*     thread,
                                 scorep_profile_node* node )
{
    SCOREP_Thread_GetProfileLocationData( thread )->current_task_node = node;
}
