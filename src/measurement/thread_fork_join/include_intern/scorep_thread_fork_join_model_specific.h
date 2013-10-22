#ifndef SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H_
#define SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H_

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct SCOREP_Location;

/*************** Model-specific threading functions ******************/
/**
 * @name Model-specific threading functions. To be implemented for
 * every supported threading model. For most of the generic thread
 * functions SCOREP_Thread_<Foo> there is a scorep_thread_on_<foo>
 * counterpart that is called by SCOREP_Thread_<Foo>.
 */
/**@{*/


/**
 * Return the current thread's SCOREP_Thread_PrivateData object. This
 * function is supposed to return a valid object, never 0. The object
 * is usually accessed via a model-specific thread-local-storage
 * mechanism.
 */
struct scorep_thread_private_data*
scorep_thread_get_private_data();


/**
 * Initialize the model-specific @a modelData part attached to the
 * SCOREP_Thread_PrivateData @a tpd object.
 */
void
scorep_thread_on_create_private_data( struct scorep_thread_private_data* tpd,
                                      void*                              modelData );


/**
 * Perform the model-specific part of the initialization of the
 * threading subsystem, e.g., make @a initialTpd available via a
 * thread-local-storage mechanism.
 */
void
scorep_thread_on_initialize( struct scorep_thread_private_data* initialTpd );


/**
 * Perform the model-specific part of the finalization of the
 * threading subsystem.
 */
void
scorep_thread_on_finalize( struct scorep_thread_private_data* initialTpd );


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
 * after the return from this function) to enbale a consistency check
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
 * Return this thread's parent scorep_thread_private_data object. This
 * functions is called once from SCOREP_ThreadForkJoin_TeamBegin()
 * only. It is called before scorep_thread_on_team_begin(). Keep this
 * in mind while implementing.
 */
struct scorep_thread_private_data*
scorep_thread_on_team_begin_get_parent( void );


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
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object. To be set.
 *
 * @param paradigm  One of the predefined threading models. Allows for
 * consistency checks.
 *
 * @param threadId Thread Id in current team. Out of [0,..,team_size). To be set.
 *
 * @param firstForkLocations Array of pre-created locations to get a mapping
 * between thread-id and location-id for the first fork only. For subsequent
 * forks, pass 0.
 */
void
scorep_thread_on_team_begin( struct scorep_thread_private_data*  parentTpd,
                             struct scorep_thread_private_data** currentTpd,
                             SCOREP_ParadigmType                 paradigm,
                             int*                                threadId,
                             struct SCOREP_Location**            firstForkLocations,
                             bool*                               locationIsCreated );


/**
 * Perform model-specific thread-end activities for fork-join as well
 * as create-wait models.  Called by SCOREP_Thread_End(). Needs to set
 * @a parentTpd to a valid SCOREP_Thread_PrivateData object.
 *
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param parentTpd Parent thread's SCOREP_Thread_PrivateData
 * object.
 */
void
scorep_thread_on_end( struct scorep_thread_private_data*  currentTpd,
                      struct scorep_thread_private_data** parentTpd,
                      SCOREP_ParadigmType                 paradigm );



/**
 * Returns the size in bytes of the model-specific object embedded into
 * each scorep_thread_private_data object.
 */
size_t
scorep_thread_get_sizeof_model_data();


/**
 * Delete all SCOREP_Thread_PrivateData objects created in
 * SCOREP_Thread_OnBegin() events, starting at @a tpd (usually a tree
 * structure).
 */
void
scorep_thread_delete_private_data( struct scorep_thread_private_data* tpd );


uint32_t
scorep_thread_get_team_size();


void
scorep_thread_create_location_name( char*                              locationName,
                                    int                                locationNameMaxLength,
                                    int                                threadId,
                                    struct scorep_thread_private_data* parentTpd );


/**
 * Return the corresponding SCOREP_ParadigmType. External/callback threading
 * models are supposed to return SCOREP_PARADIGM_THREAD_FORK_JOIN.
 */
SCOREP_ParadigmType
scorep_thread_get_paradigm( void );

/**@}*/


#endif /* SCOREP_THREAD_FORK_JOIN_MODEL_SPECIFIC_H_ */
