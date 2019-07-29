/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_PROCESS_H
#define SCOREP_PROFILE_PROCESS_H

/**
 * @file
 * @brief Contains the declaration of functions for post-processing a profile.
 *
 *
 *
 */


/**
   Substitutes collapse nodes by regular region nodes for a region named "COLLAPSE".
   Thus, collpase nodes behave like a normals region during the rest of the
   post-processing and writing. It must occur before scorep_profile_expand_threads().
 */
void
scorep_profile_process_collapse( void );

/**
   Expands all threads. All nodes of type @ref SCOREP_PROFILE_NODE_THREAD_START
   in the profile are substituted by the callpath to the node where the thread was
   activated. In OpenMP this is the callpath which contained the parallel region
   statement.
 */
void
scorep_profile_expand_threads( void );

/**
   We store the locations in the order they appear, which might be different from the
   logical numbering they get from the threading system, e.g., different from
   their omp_thread_num() value. Thus, thsi function oders the thread, thus, that the
   profile writing algorithms find them in the correct order.
   We assume that the local thread id is encoded in the left 32 bit of the location id.
 */
void
scorep_profile_sort_threads( void );

/**
   Walks through the master thread and assigns new callpath handles to those nodes
   which have no callpath handle so far.
 */
void
scorep_profile_assign_callpath_to_master( void );

/**
   Traverses all threads and matches their callpathes to the master thread. If a worker
   thread has a callpath which is nor present in the master thread. The callpath is added
   to the master thread and registered. Before executing this function you need to
   execute @ref scorep_profile_assign_callpath_to_master().
 */
void
scorep_profile_assign_callpath_to_workers( void );

/**
   Cuts out phases from the tree and makes phases to root nodes of separate trees.
 */
void
scorep_profile_process_phases( void );

/**
   Processes parameter nodes into a parameters array of the region.
 */
void
scorep_profile_process_parameters( void );

/**
   Move the visit count of task pointer metrics to the "number of switches" metric.
 */
void
scorep_profile_process_tasks( void );

/**
   Changes the visit count in all nodes from exclusive to inclusive.
 */
void
scorep_profile_inclusify_visits( void );

/**
   Aggregates the profile to contain only some key threads.
 */
void
scorep_profile_cluster_key_threads( void );

/**
   Aggregates locations that have the same calltree structure.
 */
void
scorep_profile_cluster_same_location( void );

/**
    Creates and initializes the number of threads metric. Assigns the value 1 to every
    node that has a non-zero visit count.
 */
void
scorep_profile_init_num_threads_metric( void );

#endif /* SCOREP_PROFILE_PROCESS_H */
