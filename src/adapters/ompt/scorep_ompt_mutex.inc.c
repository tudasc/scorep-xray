/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Provide hashtable for objects representing OMPT's wait_ids. They are used in
 * callbacks mutex_acquire/acquired/released. OMPT wait_ids are integers. We
 * need to create corresponding mutex objects to serve the needs of
 * SCOREP_Thread(Acquire|Release)Lock().
 */

#include <stdint.h>

#include <SCOREP_FastHashtab.h>
#include <UTILS_Mutex.h>


typedef struct mutex_table_key_t
{
    ompt_wait_id_t wait_id;
    ompt_mutex_t   kind;
} mutex_table_key_t;


typedef struct mutex_obj_t
{
    uint32_t id;
    uint32_t acquisition_order;
    union
    {
        struct /* for critical and ordered only */
        {
            SCOREP_RegionHandle outer_region;
            SCOREP_RegionHandle sblock_region;
        };
        uint32_t nest_level; /* for nest locks only */
    } optional;
    UTILS_Mutex in_release_operation;
} mutex_obj_t;

typedef mutex_obj_t* mutex_table_value_t;

/* *INDENT-OFF* */
static inline bool mutex_table_get_and_insert( mutex_table_key_t k, void* cd, mutex_table_value_t* v );
static inline bool mutex_table_get( mutex_table_key_t k, mutex_table_value_t* v );
/* *INDENT-ON* */


/* Retrieve mutex object for given wait_id and mutex-kind using
   mutex_get_and_insert() and mutex_get() only. */
static inline mutex_obj_t*
mutex_get_and_insert( ompt_wait_id_t waitId,
                      ompt_mutex_t   kind )
{
    mutex_table_key_t   key = { .wait_id = waitId, .kind = kind };
    mutex_table_value_t value;
    mutex_table_get_and_insert( key, NULL, &value );
    return value;
}

static inline mutex_obj_t*
mutex_get( ompt_wait_id_t waitId,
           ompt_mutex_t   kind )
{
    mutex_table_key_t   key = { .wait_id = waitId, .kind = kind };
    mutex_table_value_t value;
    bool                found = mutex_table_get( key, &value );
    UTILS_BUG_ON( !found, "Mutex object not in hash table, but should be." );
    return value;
}

/* Hashtable used for mapping OMPT wait_ids as seen in mutex_acquired|released
   callbacks to mutex objects to be used for SCOREP_Thread(Acquire|Release)Lock
   and corresponding Enter/Exit routines. The callbacks are triggered by critical,
   atomic (if recorded), and ordered constructs as well as locks (through runtime
   library routines).
   For locks, we have init/destroy callbacks and could insert/remove objects into
   the hash table. For critical, atomic, and ordered we lack the ability to remove
   from the hash table. We expect a limited number of locks and thus, use the
   slightly faster monotonic hash table for all wait_id use cases. */

#define MUTEX_TABLE_HASH_EXPONENT 8

static inline uint32_t
mutex_table_bucket_idx( mutex_table_key_t key )
{
    uint32_t hash_value = jenkins_hash( &key.wait_id, sizeof( key.wait_id ), 0 );
    hash_value = jenkins_hash( &key.kind, sizeof( key.kind ), hash_value );
    return hash_value & hashmask( MUTEX_TABLE_HASH_EXPONENT );
}

static inline bool
mutex_table_equals( mutex_table_key_t key1,
                    mutex_table_key_t key2 )
{
    return ( key1.wait_id == key2.wait_id ) && ( key1.kind == key2.kind );
}

static inline void*
mutex_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static mutex_table_value_t
mutex_table_value_ctor( mutex_table_key_t* key,
                        void*              ctorData )
{
    static uint32_t mutex_obj_cnt;

    mutex_table_value_t value = SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE,
                                                                   sizeof( *value ) );
    value->id = UTILS_Atomic_FetchAdd_uint32( &mutex_obj_cnt, 1,
                                              UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    value->acquisition_order      = 0;
    value->optional.outer_region  = SCOREP_INVALID_REGION;
    value->optional.sblock_region = SCOREP_INVALID_REGION;
    value->in_release_operation   = UTILS_MUTEX_INIT;
    return value;
}

static void
mutex_table_iterate_key_value_pair( mutex_table_key_t   key,
                                    mutex_table_value_t value )
{
}

static void
mutex_table_free_chunk( void* chunk )
{
}

SCOREP_HASH_TABLE_MONOTONIC( mutex_table, 5, hashsize( MUTEX_TABLE_HASH_EXPONENT ) );

#undef MUTEX_TABLE_HASH_EXPONENT
