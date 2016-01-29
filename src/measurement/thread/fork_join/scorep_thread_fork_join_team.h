/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OMP_THREAD_TEAMS_H
#define SCOREP_OMP_THREAD_TEAMS_H


/**
 * @file
 */


#include <SCOREP_Definitions.h>

#include <stdint.h>
#include <stddef.h>


struct SCOREP_Location;

extern size_t scorep_thread_fork_join_subsystem_id;

struct scorep_thread_team_data
{
    scorep_definitions_manager_entry thread_team;
    uint32_t                         team_leader_counter;
};


struct scorep_thread_team_comm_payload
{
    uint32_t num_threads;
    uint32_t thread_num;
    uint32_t singleton_counter;
};


SCOREP_InterimCommunicatorHandle
scorep_thread_get_team_handle( struct SCOREP_Location*          location,
                               SCOREP_InterimCommunicatorHandle parentThreadTeam,
                               uint32_t                         numThreads,
                               uint32_t                         threadNum );


SCOREP_InterimCommunicatorHandle
scorep_thread_get_parent_team_handle( SCOREP_InterimCommunicatorHandle threadHandle );


struct scorep_thread_team_data*
scorep_thread_fork_join_create_team_data( struct SCOREP_Location* location );


#endif /* SCOREP_OMP_THREAD_TEAMS_H */
