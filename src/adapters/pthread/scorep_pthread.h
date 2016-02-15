/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
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

#ifndef SCOREP_PTHREAD_H
#define SCOREP_PTHREAD_H

/**
 * @file
 */


#include <scorep/SCOREP_PublicTypes.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCOREP_PTHREAD_REGIONS                                         \
    SCOREP_PTHREAD_REGION( CREATE,            "create",            THREAD_CREATE ) \
    SCOREP_PTHREAD_REGION( JOIN,              "join",              THREAD_WAIT ) \
    SCOREP_PTHREAD_REGION( EXIT,              "exit",              WRAPPER ) \
    SCOREP_PTHREAD_REGION( CANCEL,            "cancel",            WRAPPER ) \
    SCOREP_PTHREAD_REGION( DETACH,            "detach",            WRAPPER ) \
    SCOREP_PTHREAD_REGION( MUTEX_INIT,        "mutex_init",        WRAPPER ) \
    SCOREP_PTHREAD_REGION( MUTEX_DESTROY,     "mutex_destroy",     WRAPPER ) \
    SCOREP_PTHREAD_REGION( MUTEX_LOCK,        "mutex_lock",        WRAPPER ) \
    SCOREP_PTHREAD_REGION( MUTEX_UNLOCK,      "mutex_unlock",      WRAPPER ) \
    SCOREP_PTHREAD_REGION( MUTEX_TRYLOCK,     "mutex_trylock",     WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_INIT,         "cond_init",         WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_SIGNAL,       "cond_signal",       WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_BROADCAST,    "cond_broadcast",    WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_WAIT,         "cond_wait",         WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_TIMEDWAIT,    "cond_timedwait",    WRAPPER ) \
    SCOREP_PTHREAD_REGION( COND_DESTROY,      "cond_destroy",      WRAPPER )



typedef enum scorep_pthread_region_types
{
#define SCOREP_PTHREAD_REGION( NAME, name, TYPE ) SCOREP_PTHREAD_ ## NAME,

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

/** Policies when to reuse a Pthread location. */
typedef enum scorep_pthread_reuse_policy_type
{
    SCOREP_PTHREAD_REUSE_POLICY_NEVER,
    SCOREP_PTHREAD_REUSE_POLICY_SAME_START_ROUTINE,
    SCOREP_PTHREAD_REUSE_POLICY_ALWAYS
} scorep_pthread_reuse_policy_type;

extern scorep_pthread_reuse_policy_type scorep_pthread_reuse_policy;

#endif /* SCOREP_PTHREAD_H */
