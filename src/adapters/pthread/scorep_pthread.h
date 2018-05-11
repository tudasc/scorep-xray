/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PTHREAD_H
#define SCOREP_PTHREAD_H

/**
 * @file
 */


#include <scorep/SCOREP_PublicTypes.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCOREP_PTHREAD_REGIONS                                         \
    SCOREP_PTHREAD_REGION( int,  pthread_create,         CREATE,            THREAD_CREATE, ( pthread_t*, const pthread_attr_t*, void* ( * )( void* ), void* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_join,           JOIN,              THREAD_WAIT,   ( pthread_t, void** ) ) \
    SCOREP_PTHREAD_REGION( void, pthread_exit,           EXIT,              WRAPPER,       ( void* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cancel,         CANCEL,            WRAPPER,       ( pthread_t ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_detach,         DETACH,            WRAPPER,       ( pthread_t ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_mutex_init,     MUTEX_INIT,        WRAPPER,       ( pthread_mutex_t*, const pthread_mutexattr_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_mutex_destroy,  MUTEX_DESTROY,     WRAPPER,       ( pthread_mutex_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_mutex_lock,     MUTEX_LOCK,        WRAPPER,       ( pthread_mutex_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_mutex_unlock,   MUTEX_UNLOCK,      WRAPPER,       ( pthread_mutex_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_mutex_trylock,  MUTEX_TRYLOCK,     WRAPPER,       ( pthread_mutex_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_init,      COND_INIT,         WRAPPER,       ( pthread_cond_t*, const pthread_condattr_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_signal,    COND_SIGNAL,       WRAPPER,       ( pthread_cond_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_broadcast, COND_BROADCAST,    WRAPPER,       ( pthread_cond_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_wait,      COND_WAIT,         WRAPPER,       ( pthread_cond_t*, pthread_mutex_t* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_timedwait, COND_TIMEDWAIT,    WRAPPER,       ( pthread_cond_t*, pthread_mutex_t*, const struct timespec* ) ) \
    SCOREP_PTHREAD_REGION( int,  pthread_cond_destroy,   COND_DESTROY,      WRAPPER,       ( pthread_cond_t* ) )


typedef enum scorep_pthread_region_types
{
#define SCOREP_PTHREAD_REGION( rettype, name, NAME, TYPE, ARGS ) SCOREP_PTHREAD_ ## NAME,

    SCOREP_PTHREAD_REGIONS

#undef SCOREP_PTHREAD_REGION

    SCOREP_PTHREAD_REGION_SENTINEL /* For internal use only. */
} scorep_pthread_region_types;


extern SCOREP_RegionHandle scorep_pthread_regions[ SCOREP_PTHREAD_REGION_SENTINEL ];

extern size_t scorep_pthread_subsystem_id;

struct scorep_pthread_wrapped_arg;

typedef struct scorep_pthread_location_data scorep_pthread_location_data;
struct scorep_pthread_location_data
{
    struct scorep_pthread_wrapped_arg* wrapped_arg;
    struct scorep_pthread_wrapped_arg* free_list;
};

#endif /* SCOREP_PTHREAD_H */
