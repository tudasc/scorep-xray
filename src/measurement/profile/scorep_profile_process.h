/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

#ifndef SCOREP_PROFILE_PROCESS_H
#define SCOREP_PROFILE_PROCESS_H

/**
 * @file scorep_profile_process.h
 * @brief Contains the declaration of functions for post-processing a profile.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

/**
   Expands all threads. All nodes of type @ref scorep_profile_node_thread_start
   in the profile are substituted by the callpath to the node where the thread was
   activated. In OpenMP this is the callpath which contained the parallel region
   statement.
 */
extern void
scorep_profile_expand_threads();

/**
   Walks through the master thread and assigns new callpath handles to those nodes
   which have no callpath handle so far.
 */
extern void
scorep_profile_assign_callpath_to_master();

/**
   Traverses all threads and matches their callpathes to the master thread. If a worker
   thread has a callpath which is nor present in the master thread. The callpath is added
   to the master thread and registered. Before executing this function you need to
   execute @ref scorep_profile_assign_callpath_to_master().
 */
extern void
scorep_profile_assign_callpath_to_workers();

#endif // SCOREP_PROFILE_PROCESS_H
