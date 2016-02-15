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

#ifndef SCOREP_PTHREAD_MUTEX_H
#define SCOREP_PTHREAD_MUTEX_H

/**
 * @file
 * @ingroup    PTHREAD
 *
 * @brief Declaration of internal functions for lock management.
 */


#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>


typedef struct scorep_pthread_mutex scorep_pthread_mutex;
struct scorep_pthread_mutex
{
    scorep_pthread_mutex* next;        /* Separate chaining with linked lists. */
    void*                 key;         /* Used as key to hash function.
                                        * uintptr_t also possible */

    uint32_t id;                       /* [0, N[ */
    uint32_t acquisition_order;
    uint32_t nesting_level;
    bool     process_shared;
};


void
scorep_pthread_mutex_init( void );


void
scorep_pthread_mutex_finalize( void );


scorep_pthread_mutex*
scorep_pthread_mutex_hash_put( pthread_mutex_t* pthreadMutex );


scorep_pthread_mutex*
scorep_pthread_mutex_hash_get( pthread_mutex_t* pthreadMutex );


void
scorep_pthread_mutex_hash_remove( pthread_mutex_t* pthreadMutex );

#endif /* SCOREP_PTHREAD_MUTEX_H */
