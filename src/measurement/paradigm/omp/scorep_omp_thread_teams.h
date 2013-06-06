/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_OMP_THREAD_TEAMS_H
#define SCOREP_OMP_THREAD_TEAMS_H


/**
 * @file       src/measurement/paradigm/omp/scorep_omp_thread_teams.h
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 * @status     alpha
 */


#include <stdlib.h>


#include <definitions/SCOREP_Definitions.h>


struct scorep_omp_location_data
{
    scorep_definitions_manager_entry thread_team;
    uint32_t                         team_leader_counter;
};


struct scorep_omp_comm_payload
{
    uint32_t num_threads;
    uint32_t thread_num;
    uint32_t singleton_counter;
};


extern size_t scorep_pomp_omp_subsystem_id;


extern SCOREP_InterimCommunicatorHandle
scorep_omp_get_thread_team_handle( SCOREP_Location*                 location,
                                   SCOREP_InterimCommunicatorHandle parentThreadTeam,
                                   uint32_t                         numThreads,
                                   uint32_t                         threadNum );


extern SCOREP_InterimCommunicatorHandle
scorep_omp_get_parent_thread_team_handle( SCOREP_InterimCommunicatorHandle threadHandle );


#endif /* SCOREP_OMP_THREAD_TEAMS_H */
