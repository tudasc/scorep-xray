/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREAD_MGMT_H
#define SCOREP_THREAD_MGMT_H

/**
 * @file
 */


#include <stdbool.h>


struct SCOREP_Location;


/**
 * Initialize the threading subsystem, call from initial thread,
 * e.g. for SCOREP_InitMeasurement(). Prepare data-structures for
 * thread-local access.
 *
 * Should at most allocate the master location and set the TPD, it should not
 * call into any substrate (OnNewLocation, OnActivation, ...).
 */
void
SCOREP_Thread_Initialize( void );


/**
 * Activates the master thread location.
 * Will call OnNewLocation and OnActivation now.
 */
void
SCOREP_Thread_ActivateLocation( struct SCOREP_Location* location,
                                struct SCOREP_Location* parent );


/**
 * Shut down the threading subsystem, call from initial thread,
 * e.g. for scorep_finalize().
 */
void
SCOREP_Thread_Finalize( void );


/**
 * Predicate that returns true if more than one thread is active when
 * this function is called. Needs a model-specific implementation.
 */
bool
SCOREP_Thread_InParallel( void );


/**
 * Return the initial/master thread's thread private data handle.
 */
struct scorep_thread_private_data*
SCOREP_Thread_GetInitialTpd( void );


/**
 * Return true if the current active thread is the initial/master thread.
 */
bool
SCOREP_Thread_IsIntialThread( void );


#endif /* SCOREP_THREAD_MGMT_H */
