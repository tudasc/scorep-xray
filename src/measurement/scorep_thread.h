/*
 * This file is part of the Score-P software (http://www.score-p.org)
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

#ifndef SCOREP_INTERNAL_THREAD_H
#define SCOREP_INTERNAL_THREAD_H



/**
 * @file       scorep_thread.h
 *
 *
 *
 */


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <SCOREP_Location.h>


typedef struct scorep_thread_private_data scorep_thread_private_data;


/***************** Generic threading functions *********************/
/**
 * @name Generic threading functions implemented by the measurement
 * core.
 *
 */
/**@{*/


/**
 * Initialize the threading subsystem, call from initial thread,
 * e.g. for SCOREP_InitMeasurement(). Prepare data-structures for
 * thread-local access.
 */
void
SCOREP_Thread_Initialize( void );


/**
 * Shut down the threading subsystem, call from initial thread,
 * e.g. for scorep_finalize().
 */
void
SCOREP_Thread_Finalize( void );


/**
 * Returns a process-unique sequence count starting a 0 for the
 * initial thread and being incremented thereafter. Values >= 1
 * identify a parallel execution started by either
 * SCOREP_Thread_Fork() or SCOREP_Thread_Create().
 */
uint32_t
scorep_thread_get_next_fork_sequence_count( void );


/**
 * Create a SCOREP_Thread_PrivateData object and corresponding model
 * data accordingly.
 *
 * @note No SCOREP_Location is created as the @a parent location might
 * get reused. If there is a need for a new SCOREP_Location object,
 * create it in the model-specific functions, usually in
 * SCOREP_Thread_OnBegin().
 */
scorep_thread_private_data*
scorep_thread_create_private_data( scorep_thread_private_data* parent,
                                   uint32_t                    forkSequenceCount );


/**
 * Returns the model-specific data of the SCOREP_Thread_PrivateData
 * object @a tpd.
 */
void*
scorep_thread_get_model_data( scorep_thread_private_data* tpd );


/**
 * Returns true if @a tpd represents the initial thread, false
 * otherwise.
 */
bool
scorep_thread_is_initial_thread( scorep_thread_private_data* tpd );


/**
 * Get the parent SCOREP_Thread_PrivateData object of @a tpd. Returns
 * 0 for the initial thread.
 */
scorep_thread_private_data*
scorep_thread_get_parent( scorep_thread_private_data* tpd );


/**
 * Returns the SCOREP_Location object currently associated with @a tpd.
 */
SCOREP_Location*
scorep_thread_get_location( scorep_thread_private_data* tpd );


/**
 * Associates a SCOREP_Location object @a location with a
 * SCOREP_Thread_PrivateData object @a tpd.
 */
void
scorep_thread_set_location( scorep_thread_private_data* tpd,
                            SCOREP_Location*            location );

/**@}*/


/*************** Model-specific threading functions ******************/
/**
 * @name Model-specific threading functions. To be implemented for
 * every supported threading model. For most of the generic thread
 * functions SCOREP_Thread_<Foo> there is a scorep_thread_on_<foo>
 * counterpart that is called by SCOREP_Thread_<Foo>.
 */
/**@{*/


/**
 * Predicate that returns true if more than one thread is active when
 * this function is called. Needs a model-specific implementation.
 */
bool
SCOREP_Thread_InParallel( void );


/**
 * Return the current thread's SCOREP_Thread_PrivateData object. This
 * function is supposed to return a valid object, never 0. The object
 * is usually accessed via a model-specific thread-local-storage
 * mechanism.
 */
scorep_thread_private_data*
scorep_thread_get_private_data();


/**
 * Initialize the model-specific @a modelData part attached to the
 * SCOREP_Thread_PrivateData @a tpd object. @a forkSequenceCount has
 * the value of the corresponding scorep_thread_create_private_data()
 * call.
 */
void
scorep_thread_on_create_private_data( scorep_thread_private_data* tpd,
                                      void*                       modelData,
                                      uint32_t                    forkSequenceCount );


/**
 * Perform the model-specific part of the initialization of the
 * threading subsystem, e.g., make @a initialTpd available via a
 * thread-local-storage mechanism.
 */
void
scorep_thread_on_initialize( scorep_thread_private_data* initialTpd );


/**
 * Perform the model-specific part of the finalization of the
 * threading subsystem.
 */
void
scorep_thread_on_finalize( scorep_thread_private_data* initialTpd );


/**
 * Perform model-specific fork activities in a fork-join model. Called
 * by SCOREP_Thread_Fork().
 *
 * @param nRequestedThreads Upper bound of threads created in the fork.
 *
 * @param forkSequenceCount Process-unique fork identifier obtained in
 * SCOREP_Thread_Fork().
 *
 * @param model One of the predefined threading models. Allows for
 * consitency checks.
 *
 * @param modelData Ready to use model-specific part of this thread's
 * SCOREP_Thread_PrivateData object.
 *
 * @param location Location object associated to this thread's
 * scorep_thread_private_data object.
 */
void
scorep_thread_on_fork( uint32_t           nRequestedThreads,
                       uint32_t           forkSequenceCount,
                       SCOREP_ThreadModel model,
                       void*              modelData,
                       SCOREP_Location*   location );


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
 * @param forkSequenceCount The forkSequenceCount from the
 * corresponding SCOREP_Thread_Fork() call. Leave unchanged if
 * provided by the model's adapter, otherwise set it to the correct
 * value.
 *
 * @param model One of the predefined threading models. Allows for
 * consitency checks.
 */
void
scorep_thread_on_join( scorep_thread_private_data*  currentTpd,
                       scorep_thread_private_data*  parentTpd,
                       scorep_thread_private_data** tpdFromNowOn,
                       uint32_t*                    forkSequenceCount,
                       SCOREP_ThreadModel           model );


/**
 * To be done when the first create-wait model gets implemented.
 */
void
scorep_thread_on_create();


/**
 * To be done when the first create-wait model gets implemented.
 */
void
scorep_thread_on_wait();


/**
 * Perform model-specific thread-begin activities for fork-join as
 * well as create-wait models.  Called by SCOREP_Thread_Begin(). Needs
 * to set @a parentTpd and @a currentTpd to valid
 * SCOREP_Thread_PrivateData objects. Needs to provide @a
 * forkSequenceCount if not provided by the model's adapter. If this
 * function creates a location via SCOREP_Location_CreateCPULocation()
 * @a locationIsCreated must be changed to true.
 *
 * @param parentTpd Parent thread's SCOREP_Thread_PrivateData
 * object.
 *
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param forkSequenceCount The forkSequenceCount from the
 * corresponding SCOREP_Thread_OnBegin() call. Leave unchanged if
 * provided by the model's adapter, otherwise set it to the correct
 * value.
 *
 * @param model  One of the predefined threading models. Allows for
 * consitency checks.
 */
void
scorep_thread_on_team_begin( scorep_thread_private_data** parentTpd,
                             scorep_thread_private_data** currentTpd,
                             uint32_t*                    forkSequenceCount,
                             SCOREP_ThreadModel           model );


/**
 * Perform model-specific thread-end activities for fork-join as well
 * as create-wait models.  Called by SCOREP_Thread_End(). Needs to set
 * @a parentTpd to a valid SCOREP_Thread_PrivateData object. Needs to
 * provide @a forkSequenceCount if not provided by the model's
 * adapter.
 *
 * @param currentTpd This thread's SCOREP_Thread_PrivateData object.
 *
 * @param parentTpd Parent thread's SCOREP_Thread_PrivateData
 * object.
 *
 * @param forkSequenceCount The forkSequenceCount from the
 * corresponding SCOREP_Thread_OnBegin() call. Leave unchanged if
 * provided by the model's adapter, otherwise set it to the correct
 * value.
 */
void
scorep_thread_on_end( scorep_thread_private_data*  currentTpd,
                      scorep_thread_private_data** parentTpd,
                      uint32_t*                    forkSequenceCount,
                      SCOREP_ThreadModel           model );


SCOREP_InterimCommunicatorHandle
scorep_thread_get_thread_team( scorep_thread_private_data* tpd );


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
scorep_thread_delete_private_data( scorep_thread_private_data* tpd );


/**@}*/

#endif /* SCOREP_INTERNAL_THREAD_H */
