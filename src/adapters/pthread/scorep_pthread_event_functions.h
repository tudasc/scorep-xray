/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_PTHREAD_EVENT_FUNCTIONS_H
#define SCOREP_PTHREAD_EVENT_FUNCTIONS_H

/**
 * Declaration of all __real_* functions used by the pthread library wrapper
 */

#include <pthread.h>

int __real_pthread_create( pthread_t*,
                           const pthread_attr_t*,
                           void* ( * )( void* ),
                           void* );

int __real_pthread_join( pthread_t,
                         void** );

void
__real_pthread_exit( void* );

int __real_pthread_cancel( pthread_t );

int __real_pthread_detach( pthread_t );

int
__real_pthread_mutex_init( pthread_mutex_t*,
                           const pthread_mutexattr_t* );

int
__real_pthread_mutex_destroy( pthread_mutex_t* );

int
__real_pthread_mutex_lock( pthread_mutex_t* mutex );

int
__real_pthread_mutex_unlock( pthread_mutex_t* mutex );

int
__real_pthread_mutex_trylock( pthread_mutex_t* mutex );

int
__real_pthread_cond_init( pthread_cond_t*,
                          const pthread_condattr_t* );

int
__real_pthread_cond_signal( pthread_cond_t* );

int
__real_pthread_cond_broadcast( pthread_cond_t* );

int
__real_pthread_cond_wait( pthread_cond_t*,
                          pthread_mutex_t* );

int
__real_pthread_cond_timedwait( pthread_cond_t*,
                               pthread_mutex_t*,
                               const struct timespec* );

int
__real_pthread_cond_destroy( pthread_cond_t* );

#endif /* SCOREP_PTHREAD_EVENT_FUNCTIONS_H */
