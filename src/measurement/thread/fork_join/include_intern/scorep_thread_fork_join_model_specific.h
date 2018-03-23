/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2013-2014, 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H
#define SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct SCOREP_Location;
struct scorep_thread_private_data;

void
scorep_thread_create_mutexes( void );


void
scorep_thread_destroy_mutexes( void );


/**
 * Perform model-specific fork activities in a fork-join model. Called
 * by SCOREP_Thread_Fork().
 *
 * @param nRequestedThreads Upper bound of threads created in the fork.
 *
 * @param paradigm One of the predefined threading models. Allows for
 * consistency checks.
 *
 * @param modelData Ready to use model-specific part of this thread's
 * SCOREP_Thread_PrivateData object.
 *
 * @param location Location object associated to this thread's
 * scorep_thread_private_data object.
 */
void
scorep_thread_on_fork( uint32_t                nRequestedThreads,
                       SCOREP_ParadigmType     paradigm,
                       void*                   modelData,
                       struct SCOREP_Location* location );


/**
 * Perform model-specific join activities in a fork-join model. Called
 * by SCOREP_Thread_Join(). Needs to provide the
 * SCOREP_Thread_PrivateData object @a tpdFromNowOn (that will be used
 * after the return from this function) to enable a consistency check
 * in the calling function. Needs to provide @a forkSequenceCount if
 * not provided by the model's adapter.
 *
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param parentTpd Parent thread's SCOREP_Thread_PrivateData
 * object.
 *
 * @param tpdFromNowOn The SCOREP_Thread_PrivateData object that will
 * be used after the return from this function via
 * SCOREP_Thread_GetPrivateData().
 *
 * @param paradigm One of the predefined threading models. Allows for
 * consistency checks.
 */
void
scorep_thread_on_join( struct scorep_thread_private_data*  currentTpd,
                       struct scorep_thread_private_data*  parentTpd,
                       struct scorep_thread_private_data** tpdFromNowOn,
                       SCOREP_ParadigmType                 paradigm );


/**
 * Retrieve this thread's @a parent either by walking the internal tree
 * @a nestingLevel steps using @a ancestorInfo as array of indices
 * starting from the master thread into the children arrays. If
 * @a nestingLevel equals 0, interpret @a ancestorInfo as the parent.
 */
void
scorep_thread_on_team_begin_get_parent( uint32_t                            nestingLevel,
                                        void*                               ancestorInfo,
                                        struct scorep_thread_private_data** parent );

/**
 * Perform model-specific thread-begin activities for fork-join as
 * well as create-wait models.  Called by SCOREP_Thread_Begin(). Needs
 * to set @a currentTpd to a valid SCOREP_Thread_PrivateData object. If
 * this function creates a location via SCOREP_Location_CreateCPULocation()
 * @a locationIsCreated must be changed to true.
 *
 * @param parentTpd Parent thread's SCOREP_Thread_PrivateData
 * object.
 *
 * @param[out] currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param paradigm One of the fork-join threading models.
 *
 * @param threadId Thread Id in current team. Out of [0,..,teamSize).
 *
 * @param teamSize Number of threads that constitute the parallel region.
 *
 * @param firstForkLocations Array of pre-created locations to get a mapping
 * between thread-id and location-id for the first fork only. For subsequent
 * forks, pass 0.
 *
 * @param[out] locationIsCreated Indicates that the location was newly created
 * for this thread.
 */
void
scorep_thread_on_team_begin( struct scorep_thread_private_data*  parentTpd,
                             struct scorep_thread_private_data** currentTpd,
                             SCOREP_ParadigmType                 paradigm,
                             uint32_t                            threadId,
                             uint32_t                            teamSize,
                             struct SCOREP_Location**            firstForkLocations,
                             bool*                               locationIsCreated );


/**
 * Perform model-specific thread-end activities for fork-join as well
 * as create-wait models.  Called by SCOREP_Thread_End(). Needs to set
 * @a parentTpd to a valid SCOREP_Thread_PrivateData object.
 *
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param[out] parentTpd[] Parent thread's SCOREP_Thread_PrivateData
 * object.
 *
 * @param threadId Thread Id out of [0..teamSize) within the team of threads that
 * constitute the parallel region. 0 is defined to be the master thread, which
 * forked this thread team.
 *
 * @param teamSize Number of threads that constitute the parallel region.
 *
 * @param paradigm One of the fork-join threading models.
 */
void
scorep_thread_on_team_end( struct scorep_thread_private_data*  currentTpd,
                           struct scorep_thread_private_data** parentTpd,
                           uint32_t                            threadId,
                           uint32_t                            teamSize,
                           SCOREP_ParadigmType                 paradigm );


/**
 *
 *
 * @param locationName
 * @param locationNameMaxLength
 * @param threadId
 * @param parentTpd
 */
void
scorep_thread_create_location_name( char*                              locationName,
                                    size_t                             locationNameMaxLength,
                                    uint32_t                           threadId,
                                    struct scorep_thread_private_data* parentTpd );


/**@}*/


#endif /* SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H */
