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

/**
 * @file
 * @ingroup    PTHREAD
 *
 * @brief Implementation of internal functions for lock management.
 */

#include <config.h>

#include "scorep_pthread_mutex.h"

#define SCOREP_DEBUG_MODULE_NAME PTHREAD
#include <UTILS_Debug.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>

#include <UTILS_Error.h>
#include <jenkins_hash.h>

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


/* *INDENT-OFF* */
static inline scorep_pthread_mutex* mutex_hash_get( pthread_mutex_t* pthreadMutex );
static inline void* get_key( pthread_mutex_t* pthreadMutex );
static inline uint32_t get_bucket( pthread_mutex_t* pthreadMutex );
/* *INDENT-ON*  */


static SCOREP_Mutex hash_table_mutex;
/* hash_table_size = 2^^HASH_TABLE_SIZE_EXP */
#define HASH_TABLE_SIZE_EXP 8
#define HASH_INITIAL ( uint32_t )0
static scorep_pthread_mutex* hash_table[ hashsize( HASH_TABLE_SIZE_EXP ) ];
static int32_t               mutex_id = 0;
static scorep_pthread_mutex* free_list_head;

void
scorep_pthread_mutex_init( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &hash_table_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
}


void
scorep_pthread_mutex_finalize( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &hash_table_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


scorep_pthread_mutex*
scorep_pthread_mutex_hash_put( pthread_mutex_t* pthreadMutex )
{
    SCOREP_MutexLock( hash_table_mutex );
    scorep_pthread_mutex* new_mutex = mutex_hash_get( pthreadMutex );
    if ( !new_mutex )
    {
        if ( free_list_head )
        {
            new_mutex      = free_list_head;
            free_list_head = free_list_head->next;
        }
        else
        {
            new_mutex = SCOREP_Memory_AllocForMisc( sizeof( scorep_pthread_mutex ) );
        }
        memset( new_mutex, 0, sizeof( *new_mutex ) );
        UTILS_BUG_ON( !new_mutex, "Failed to allocate memory for scorep_pthread_mutex object." );
        new_mutex->key               = get_key( pthreadMutex );
        new_mutex->id                = mutex_id++;
        new_mutex->acquisition_order = 0;
        new_mutex->nesting_level     = 0;
        new_mutex->process_shared    = false;

        /* insert at front */
        uint32_t bucket = get_bucket( pthreadMutex );
        new_mutex->next      = hash_table[ bucket ];
        hash_table[ bucket ] = new_mutex;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PTHREAD, "Mutex:%" PRIu32 " in bucket:%"
                            PRIu32 "", new_mutex->id, bucket );
    }
    SCOREP_MutexUnlock( hash_table_mutex );
    return new_mutex;
}


static inline scorep_pthread_mutex*
mutex_hash_get( pthread_mutex_t* pthreadMutex )
{
    scorep_pthread_mutex* iterator = hash_table[ get_bucket( pthreadMutex ) ];
    const void*           key      = get_key( pthreadMutex );
    while ( iterator != 0 )
    {
        if ( key == iterator->key )
        {
            break;
        }
        iterator = iterator->next;
    }
    return iterator;
}


static inline void*
get_key( pthread_mutex_t* pthreadMutex )
{
    return &( *pthreadMutex );
}


static inline uint32_t
get_bucket( pthread_mutex_t* pthreadMutex )
{
    void*    key[]  = { get_key( pthreadMutex ) };
    uint32_t bucket = jenkins_hash( key, sizeof( void* ), HASH_INITIAL )
                      & hashmask( HASH_TABLE_SIZE_EXP );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PTHREAD, "key    :%p", key[ 0 ] );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PTHREAD, "bucket :%" PRIu32 "", bucket );

    return bucket;
}


scorep_pthread_mutex*
scorep_pthread_mutex_hash_get( pthread_mutex_t* pthreadMutex )
{
    SCOREP_MutexLock( hash_table_mutex );
    scorep_pthread_mutex* iterator = mutex_hash_get( pthreadMutex );
    SCOREP_MutexUnlock( hash_table_mutex );

    return iterator;
}


void
scorep_pthread_mutex_hash_remove( pthread_mutex_t* pthreadMutex )
{
    SCOREP_MutexLock( hash_table_mutex );
    uint32_t              bucket   = get_bucket( pthreadMutex );
    scorep_pthread_mutex* iterator = hash_table[ bucket ];
    if ( iterator )
    {
        const void* key = get_key( pthreadMutex );
        if ( key == iterator->key )
        {
            /* delete first element */
            hash_table[ bucket ] = iterator->next;
            iterator->next       = free_list_head;
            free_list_head       = iterator;
        }
        else
        {
            while ( iterator->next && key != iterator->next->key )
            {
                iterator = iterator->next;
            }
            if ( iterator->next )
            {
                scorep_pthread_mutex* delete_me = iterator->next;
                iterator->next  = delete_me->next;
                delete_me->next = free_list_head;
                free_list_head  = delete_me;
            }
            else
            {
                UTILS_WARNING( "Pthread mutex not in hash table." );
            }
        }
    }
    else
    {
        UTILS_WARNING( "Pthread mutex not in hash table." );
    }
    SCOREP_MutexUnlock( hash_table_mutex );
}
