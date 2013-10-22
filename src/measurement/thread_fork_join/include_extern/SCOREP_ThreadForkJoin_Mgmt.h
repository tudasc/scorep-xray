#ifndef SCOREP_THREAD_FORK_JOIN_MGMT_H_
#define SCOREP_THREAD_FORK_JOIN_MGMT_H_

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


#include <stdbool.h>

struct SCOREP_Location;

/**
 * Initialize the threading subsystem, call from initial thread,
 * e.g. for SCOREP_InitMeasurement(). Prepare data-structures for
 * thread-local access.
 */
void
SCOREP_ThreadForkJoin_Initialize( void );


/**
 * Shut down the threading subsystem, call from initial thread,
 * e.g. for scorep_finalize().
 */
void
SCOREP_ThreadForkJoin_Finalize( void );


/**
 * Predicate that returns true if more than one thread is active when
 * this function is called. Needs a model-specific implementation.
 */
bool
SCOREP_Thread_InParallel( void );


#endif /* SCOREP_THREAD_FORK_JOIN_MGMT_H_ */
