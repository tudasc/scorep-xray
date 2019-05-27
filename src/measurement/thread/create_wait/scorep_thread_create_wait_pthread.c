/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016, 2018-2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>

#include <scorep_thread_create_wait_model_specific.h>
#include <scorep_thread_model_specific.h>
#include <scorep_thread_generic.h>
#include <SCOREP_ThreadCreateWait_Event.h>

#include <scorep_location_management.h>

#define SCOREP_DEBUG_MODULE_NAME PTHREAD
#include <UTILS_Debug.h>

#include <UTILS_Error.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Properties.h>
#include <SCOREP_Task.h>
#include <SCOREP_InMeasurement.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


/* *INDENT-OFF* */
static void create_tpd_key( void );
static struct scorep_thread_private_data* pop_from_tpd_reuse_pool( uintptr_t );
static void push_to_tpd_reuse_pool( struct scorep_thread_private_data* );
static void tpd_dtor( void* );
static void create_orphan_key( void );
static void orphan_dtor( void* );
static struct scorep_thread_private_data* create_orphan_thread( void );
/* *INDENT-ON*  */


static pthread_key_t tpd_key;
static pthread_once_t tpd_key_once = PTHREAD_ONCE_INIT;
static pthread_key_t  orphan_key;
static pthread_once_t orphan_key_once = PTHREAD_ONCE_INIT;

typedef struct private_data_pthread private_data_pthread;
struct private_data_pthread
{
    uintptr_t tpd_reuse_key;
};

struct reuse_pool_tpd
{
    struct reuse_pool_tpd*             next;
    struct scorep_thread_private_data* tpd;
};

struct tpd_reuse_pool
{
    struct tpd_reuse_pool* next;
    uintptr_t              reuse_key;
    struct reuse_pool_tpd* unused_tpds;
};

#define TPD_REUSE_POOL_SHIFT  5
#define TPD_REUSE_POOL_SIZE   ( 1 << TPD_REUSE_POOL_SHIFT )
#define TPD_REUSE_POOL_MASK   ( TPD_REUSE_POOL_SIZE -  1 )
static struct tpd_reuse_pool  tpd_reuse_pool[ TPD_REUSE_POOL_SIZE ];
static struct reuse_pool_tpd* tpd_reuse_pool_free_list;
static SCOREP_Mutex           tpd_reuse_pool_mutex;

static SCOREP_Mutex pthread_location_count_mutex;
static SCOREP_Mutex orphan_location_count_mutex;

struct SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation( void )
{
    UTILS_DEBUG_ENTRY();
    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();

    if ( tpd == 0 )
    {
        UTILS_DEBUG( "tpd == NULL, creating orphan thread" );
        tpd = create_orphan_thread();
    }

    struct SCOREP_Location* location = scorep_thread_get_location( tpd );
    UTILS_BUG_ON( location == 0, "Invalid location object associated with "
                  "Pthread thread specific data object." );
    return location;
}


struct scorep_thread_private_data*
scorep_thread_get_private_data( void )
{
    UTILS_DEBUG_ENTRY();
    return pthread_getspecific( tpd_key );
}


static struct scorep_thread_private_data*
create_orphan_thread( void )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_Location* location;
    scorep_thread_create_wait_orphan_begin( &location );

    /* An orphan thread needs to trigger Score-P when it is terminated.
     * Cleanup handlers or dtors of thread-specific keys might be used for
     * this task.
     * The pthread_cleanup_push man page states: "Clean-up handlers are not
     * called if the thread terminates by performing a return from the thread
     * start function."
     * The pthread_key_create man page states: "When a thread terminates via
     * pthread_exit or by cancellation, destr_function is called, ..."
     * The pthread_exit man page states: "Performing a return from the start
     * function of any thread other than the main thread results in an
     * implicit call to pthread_exit(), ..."
     * I.e., by using a dtor of a thread-specific key we intercept all possible
     * ways a thread can terminate. */
    pthread_setspecific( orphan_key, location );

    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();
    UTILS_BUG_ON( tpd == NULL, "Creation of orphan thread data structure failed." );
    return tpd;
}


void
scorep_thread_on_initialize( struct scorep_thread_private_data* initialTpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !initialTpd, "Invalid input data initialTpd" );

    int status = pthread_once( &tpd_key_once, create_tpd_key );
    UTILS_BUG_ON( status != 0, "Failed to create pthread_key_t object via "
                  "pthread_once()." );

    status = pthread_setspecific( tpd_key, initialTpd );
    UTILS_BUG_ON( status != 0, "Failed to store Pthread thread specific data." );

    status = pthread_once( &orphan_key_once, create_orphan_key );
    UTILS_BUG_ON( status != 0, "Failed to create pthread_key_t object via "
                  "pthread_once()." );

    status = pthread_setspecific( orphan_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to store Pthread thread specific data." );

    SCOREP_ErrorCode result = SCOREP_MutexCreate( &tpd_reuse_pool_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Can't create mutex for location reuse pool." );

    result = SCOREP_MutexCreate( &pthread_location_count_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Can't create mutex for pthread location count." );

    result = SCOREP_MutexCreate( &orphan_location_count_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Can't create mutex for orphan location count." );
}


static void
create_tpd_key( void )
{
    UTILS_DEBUG_ENTRY();
    int status = pthread_key_create( &tpd_key, &tpd_dtor );
    UTILS_BUG_ON( status != 0, "Failed to allocate a new pthread_key_t." );
}


static void
tpd_dtor( void* tpd )
{
    UTILS_DEBUG_ENTRY();
    /* Order of dtor execution is not deterministic, but we need a valid
     * tpd_key in orphan_dtor(). Re-assign tpd_key if orphan_dtor()
     * is going to be called, i.e. orphan != NULL. For non-orphan
     * threads, orphan == NULL. */
    void* orphan = pthread_getspecific( orphan_key );
    if ( orphan )
    {
        pthread_setspecific( tpd_key, tpd );
    }
}


static void
create_orphan_key( void )
{
    UTILS_DEBUG_ENTRY();
    int status = pthread_key_create( &orphan_key, &orphan_dtor );
    UTILS_BUG_ON( status != 0, "Failed to allocate a new pthread_key_t." );
}


static void
orphan_dtor( void* location )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY();

    /* Orphan threads ending after WITHIN are handled by scorep_finalize(). */
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        UTILS_DEBUG_EXIT();
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    void* terminate = SCOREP_ThreadCreateWait_TryTerminate( location );
    if ( terminate )
    {
        UTILS_BUG_ON( location == NULL, "Valid location object needed." );
        void* tpd = pthread_getspecific( tpd_key );
        UTILS_BUG_ON( tpd == NULL, "tpd must be valid during clean up of orphan thread." );

        /* The thread's start_routine will not trigger an exit event if the thread is
         * ended via pthread_cancel or pthread_exit. Close the region manually. */
        SCOREP_Task_ExitAllRegions( location,
                                    SCOREP_Task_GetCurrentTask( location ) );
        scorep_thread_create_wait_orphan_end( terminate );
    }
    UTILS_DEBUG_EXIT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_thread_on_finalize( struct scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    int status = pthread_key_delete( tpd_key );
    UTILS_BUG_ON( status != 0, "Failed to delete a pthread_key_t." );

    SCOREP_MutexDestroy( &tpd_reuse_pool_mutex );
    SCOREP_MutexDestroy( &pthread_location_count_mutex );
    SCOREP_MutexDestroy( &orphan_location_count_mutex );
}


size_t
scorep_thread_get_sizeof_model_data( void )
{
    UTILS_DEBUG_ENTRY();
    return sizeof( private_data_pthread );
}


void
scorep_thread_on_create_private_data( struct scorep_thread_private_data* tpd,
                                      void*                              modelData )
{
    UTILS_DEBUG_ENTRY();
}


void
scorep_thread_delete_private_data( struct scorep_thread_private_data* initialTpd )
{
}


void
scorep_thread_create_wait_on_create( void*                   modelData,
                                     struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
}


void
scorep_thread_create_wait_on_wait( void*                   modelData,
                                   struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
}


void
scorep_thread_create_wait_on_begin( struct scorep_thread_private_data*  parentTpd,
                                    uint32_t                            sequenceCount,
                                    uintptr_t                           locationReuseKey,
                                    struct scorep_thread_private_data** currentTpd,
                                    bool*                               locationIsCreated )
{
    UTILS_DEBUG_ENTRY();

    static unsigned pthread_location_count;
    *locationIsCreated = false;

    *currentTpd = pop_from_tpd_reuse_pool( locationReuseKey );

    if ( !*currentTpd )
    {
        /* No tpd to reuse available. Create new tpd and location. */
        SCOREP_MutexLock( pthread_location_count_mutex );
        unsigned location_count = ++pthread_location_count;
        SCOREP_MutexUnlock( pthread_location_count_mutex );

        const int provided_length = 80;
        char      location_name[ provided_length ];
        int       real_length = snprintf( location_name, provided_length, "Pthread thread %d", location_count );
        UTILS_ASSERT( real_length <  provided_length );

        SCOREP_Location* location = SCOREP_Location_CreateCPULocation( location_name );
        *locationIsCreated = true;
        *currentTpd        =  scorep_thread_create_private_data( parentTpd, location );
        scorep_thread_set_location( *currentTpd, location );
    }
    else
    {
        /* Reuse tpd and its location, update parent. */
        scorep_thread_set_parent( *currentTpd, parentTpd );
    }

    int status = pthread_setspecific( tpd_key, *currentTpd );
    UTILS_BUG_ON( status != 0, "Failed to store Pthread thread specific data." );

    /* Set/update reuse key, used at _on_end() time. */
    private_data_pthread* model_data = scorep_thread_get_model_data( *currentTpd );
    model_data->tpd_reuse_key = locationReuseKey;

    UTILS_DEBUG_EXIT();
}


static struct scorep_thread_private_data*
pop_from_tpd_reuse_pool( uintptr_t reuseKey )
{
    UTILS_DEBUG_ENTRY();
    struct scorep_thread_private_data* tpd_to_reuse = NULL;
    if ( reuseKey )
    {
        SCOREP_MutexLock( tpd_reuse_pool_mutex );

        /* find a tpd to reuse */
        size_t                 hash        = SCOREP_Hashtab_HashPointer( ( void* )reuseKey );
        size_t                 hash_bucket = hash & TPD_REUSE_POOL_MASK;
        struct tpd_reuse_pool* pool        = &tpd_reuse_pool[ hash_bucket ];
        while ( pool )
        {
            if ( pool->reuse_key == reuseKey )
            {
                /* A pool for the reuse key exists, lets see if there is an unused tpd.
                 */
                if ( pool->unused_tpds )
                {
                    /* Yes, take it out and put the now unused reuse_pool_tpd
                     * into the free list
                     */
                    struct reuse_pool_tpd* pool_tpd = pool->unused_tpds;
                    pool->unused_tpds        = pool_tpd->next;
                    tpd_to_reuse             = pool_tpd->tpd;
                    pool_tpd->tpd            = NULL;
                    pool_tpd->next           = tpd_reuse_pool_free_list;
                    tpd_reuse_pool_free_list = pool_tpd;

                    SCOREP_InvalidateProperty( SCOREP_PROPERTY_PTHREAD_LOCATION_REUSED );

                    break;
                }
            }

            pool = pool->next;
        }
        SCOREP_MutexUnlock( tpd_reuse_pool_mutex );
    }
    UTILS_DEBUG_EXIT();
    return tpd_to_reuse;
}


void
scorep_thread_create_wait_on_end( struct scorep_thread_private_data* parentTpd,
                                  struct scorep_thread_private_data* currentTpd,
                                  uint32_t                           sequenceCount )
{
    UTILS_DEBUG_ENTRY();

    int status = pthread_setspecific( tpd_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to reset Pthread thread specific data." );

    push_to_tpd_reuse_pool( currentTpd );

    UTILS_DEBUG_EXIT();
}


static void
push_to_tpd_reuse_pool( struct scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    /* tpd not needed anymore, maintain for reuse in the future */

    private_data_pthread* model_data = scorep_thread_get_model_data( tpd );
    uintptr_t             reuse_key  = model_data->tpd_reuse_key;

    if ( reuse_key )
    {
        /* Returning the tpd into the pool identified by reuse_key */
        SCOREP_MutexLock( tpd_reuse_pool_mutex );

        size_t                 hash        = SCOREP_Hashtab_HashPointer( ( void* )reuse_key );
        size_t                 hash_bucket = hash & TPD_REUSE_POOL_MASK;
        struct tpd_reuse_pool* pool        = &tpd_reuse_pool[ hash_bucket ];
        SCOREP_Location*       location    = scorep_thread_get_location( tpd );

        /* Find the pool */
        while ( pool )
        {
            if ( pool->reuse_key == reuse_key )
            {
                break;
            }
            pool = pool->next;
        }
        if ( !pool )
        {
            /* No pool yet, try the first one, which is marked with 0 as the
             * reuse key if it is still unused
             */
            pool = &tpd_reuse_pool[ hash_bucket ];
            if ( pool->reuse_key )
            {
                /* Really no pool yet, create a new one, taking memory
                 * from the just ended location
                 */
                pool = SCOREP_Location_AllocForMisc( location, sizeof( *pool ) );
                memset( pool, 0, sizeof( *pool ) );
                pool->next                         = tpd_reuse_pool[ hash_bucket ].next;
                tpd_reuse_pool[ hash_bucket ].next = pool;
            }

            /* We are occupying the pool now */
            pool->reuse_key = reuse_key;
        }

        /* We now have a pool, now we hook the location in the unused
         * locations list
         */
        struct reuse_pool_tpd* pool_tpd = tpd_reuse_pool_free_list;
        if ( pool_tpd )
        {
            tpd_reuse_pool_free_list = pool_tpd->next;
        }
        else
        {
            pool_tpd =
                SCOREP_Location_AllocForMisc( location, sizeof( *pool_tpd ) );
        }
        pool_tpd->tpd     = tpd;
        pool_tpd->next    = pool->unused_tpds;
        pool->unused_tpds = pool_tpd;

        SCOREP_MutexUnlock( tpd_reuse_pool_mutex );
    }
    UTILS_DEBUG_EXIT();
}


void
scorep_thread_create_wait_on_orphan_begin( struct scorep_thread_private_data** currentTpd,
                                           bool*                               locationIsCreated )
{
    UTILS_DEBUG_ENTRY();

    static unsigned orphan_location_count;
    *locationIsCreated = false;

    uintptr_t reuse_key = scorep_thread_create_wait_get_reuse_key( SCOREP_PARADIGM_ORPHAN_THREAD, 0 );
    *currentTpd = pop_from_tpd_reuse_pool( reuse_key );

    if ( !*currentTpd )
    {
        /* No tpd to reuse available. Create new tpd and location. */
        SCOREP_MutexLock( orphan_location_count_mutex );
        unsigned location_count = ++orphan_location_count;
        SCOREP_MutexUnlock( orphan_location_count_mutex );

        char location_name[ 80 ];
        int  length = snprintf( location_name, 80, "Orphan thread %d", location_count );
        UTILS_ASSERT( length < 80 );

        SCOREP_Location* location = SCOREP_Location_CreateCPULocation( location_name );
        *locationIsCreated = true;
        *currentTpd        =  scorep_thread_create_private_data( NULL /* parent */, location );
        scorep_thread_set_location( *currentTpd, location );
    }
    else
    {
        /* Reuse tpd and its location, update parent. */
        scorep_thread_set_parent( *currentTpd, NULL /* parent */ );
    }

    int status = pthread_setspecific( tpd_key, *currentTpd );
    UTILS_BUG_ON( status != 0, "Failed to store Pthread thread specific data." );

    /* Set/update reuse key, used at _on_end() time. */
    private_data_pthread* model_data = scorep_thread_get_model_data( *currentTpd );
    model_data->tpd_reuse_key = reuse_key;

    UTILS_DEBUG_EXIT();
}


void
scorep_thread_create_wait_on_orphan_end( struct scorep_thread_private_data* currentTpd )
{
    UTILS_DEBUG_ENTRY();

    int status = pthread_setspecific( tpd_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to reset Pthread thread specific data." );
    status = pthread_setspecific( orphan_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to reset Pthread thread specific data." );

    push_to_tpd_reuse_pool( currentTpd );

    UTILS_DEBUG_EXIT();
}


SCOREP_ParadigmType
scorep_thread_get_paradigm( void )
{
    return SCOREP_PARADIGM_PTHREAD;
}
