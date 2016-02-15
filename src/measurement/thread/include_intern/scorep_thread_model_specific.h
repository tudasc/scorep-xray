/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREAD_MODEL_SPECIFIC_H
#define SCOREP_THREAD_MODEL_SPECIFIC_H

/**
 * @file
 */

#include <SCOREP_Types.h>

#include <stddef.h>

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
scorep_thread_get_private_data( void );


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
 * Returns the size in bytes of the model-specific object embedded into
 * each scorep_thread_private_data object.
 */
size_t
scorep_thread_get_sizeof_model_data( void );


/**
 * Delete all SCOREP_Thread_PrivateData objects created in
 * SCOREP_Thread_OnBegin() events, starting at @a tpd (usually a tree
 * structure).
 */
void
scorep_thread_delete_private_data( struct scorep_thread_private_data* tpd );


/**
 * Return the corresponding SCOREP_ParadigmType.
 */
SCOREP_ParadigmType
scorep_thread_get_paradigm( void );



#endif /* SCOREP_THREAD_MODEL_SPECIFIC_H */
