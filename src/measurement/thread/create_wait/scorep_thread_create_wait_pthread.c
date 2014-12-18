/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
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

#include <scorep_thread_model_specific.h>
#include <scorep_thread_generic.h>

#include <scorep_location.h>

#define SCOREP_DEBUG_MODULE_NAME PTHREAD
#include <UTILS_Debug.h>

#include <UTILS_Error.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Properties.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>


/* *INDENT-OFF* */
static void create_tpd_key( void );
/* *INDENT-ON*  */


static pthread_key_t tpd_key;
static pthread_once_t tpd_key_once = PTHREAD_ONCE_INIT;

typedef struct private_data_pthread private_data_pthread;
struct private_data_pthread
{
    size_t location_reuse_key;
};

struct reuse_pool_location
{
    struct reuse_pool_location* next;
    SCOREP_Location*            location;
};

struct location_reuse_pool
{
    struct location_reuse_pool* next;
    size_t                      reuse_key;
    struct reuse_pool_location* unused_locations;
};

#define LOCATION_REUSE_POOL_SHIFT  5
#define LOCATION_REUSE_POOL_SIZE   ( 1 << LOCATION_REUSE_POOL_SHIFT )
#define LOCATION_REUSE_POOL_MASK   ( LOCATION_REUSE_POOL_SIZE -  1 )
static struct location_reuse_pool  location_reuse_pool[ LOCATION_REUSE_POOL_SIZE ];
static struct reuse_pool_location* location_reuse_pool_free_list;
static SCOREP_Mutex                location_reuse_pool_mutex;

struct SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation( void )
{
    UTILS_DEBUG_ENTRY();
    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();
    UTILS_BUG_ON( tpd == 0, "Invalid Pthread thread specific data object. "
                  "Please ensure that all pthread_create calls are instrumented." );
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

    SCOREP_ErrorCode result = SCOREP_MutexCreate( &location_reuse_pool_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Can't create mutex for location reuse pool." );
}


static void
create_tpd_key( void )
{
    UTILS_DEBUG_ENTRY();
    int status = pthread_key_create( &tpd_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to allocate a new pthread_key_t." );
}


void
scorep_thread_on_finalize( struct scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    int status = pthread_key_delete( tpd_key );
    UTILS_BUG_ON( status != 0, "Failed to delete a pthread_key_t." );

    SCOREP_MutexDestroy( &location_reuse_pool_mutex );
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
scorep_thread_create_wait_on_create( SCOREP_ParadigmType     paradigm,
                                     void*                   modelData,
                                     struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_PTHREAD, "Passed paradigm != "
                  "SCOREP_PARADIGM_PTHREAD." );
}


void
scorep_thread_create_wait_on_wait( SCOREP_ParadigmType     paradigm,
                                   void*                   modelData,
                                   struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_PTHREAD, "Passed paradigm != "
                  "SCOREP_PARADIGM_PTHREAD."  );
}


void
scorep_thread_create_wait_on_begin( SCOREP_ParadigmType                 paradigm,
                                    struct scorep_thread_private_data*  parentTpd,
                                    uint32_t                            sequenceCount,
                                    size_t                              locationReuseKey,
                                    struct scorep_thread_private_data** currentTpd,
                                    bool*                               locationIsCreated )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_PTHREAD, "Passed paradigm != "
                  "SCOREP_PARADIGM_PTHREAD."  );

    SCOREP_Location* location = NULL;
    *locationIsCreated = false;

    if ( locationReuseKey )
    {
        SCOREP_MutexLock( location_reuse_pool_mutex );

        /* find a location to reuse */
        size_t                      hash        = SCOREP_Hashtab_HashPointer( ( void* )locationReuseKey );
        size_t                      hash_bucket = hash & LOCATION_REUSE_POOL_MASK;
        struct location_reuse_pool* pool        = &location_reuse_pool[ hash_bucket ];
        while ( pool )
        {
            if ( pool->reuse_key == locationReuseKey )
            {
                /* A pool for the reuse key exists, lets see if there is an unused
                 * location
                 */
                if ( pool->unused_locations )
                {
                    /* Yes, take it out and put the now unused reuse_pool_location
                     * into the free list
                     */
                    struct reuse_pool_location* pool_location = pool->unused_locations;
                    pool->unused_locations        = pool_location->next;
                    location                      = pool_location->location;
                    pool_location->location       = NULL;
                    pool_location->next           = location_reuse_pool_free_list;
                    location_reuse_pool_free_list = pool_location;

                    SCOREP_InvalidateProperty( SCOREP_PROPERTY_PTHREAD_LOCATION_REUSED );

                    break;
                }
            }

            pool = pool->next;
        }
        SCOREP_MutexUnlock( location_reuse_pool_mutex );
    }

    /* We did not reused a location, create new one */
    if ( !location )
    {
        char location_name[ 80 ];
        int  length = snprintf( location_name, 80, "Pthread thread %d", sequenceCount );
        UTILS_ASSERT( length < 80 );

        location = SCOREP_Location_CreateCPULocation( location_name );

        *locationIsCreated = true;
    }

    *currentTpd =  scorep_thread_create_private_data( parentTpd,
                                                      location );
    scorep_thread_set_location( *currentTpd, location );
    int status = pthread_setspecific( tpd_key, *currentTpd );
    UTILS_BUG_ON( status != 0, "Failed to store Pthread thread specific data." );

    /* We need the reuse key at _on_end() time. */
    private_data_pthread* model_data = scorep_thread_get_model_data( *currentTpd );
    model_data->location_reuse_key = locationReuseKey;
}


void
scorep_thread_create_wait_on_end( SCOREP_ParadigmType                paradigm,
                                  struct scorep_thread_private_data* parentTpd,
                                  struct scorep_thread_private_data* currentTpd,
                                  uint32_t                           sequenceCount )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_PTHREAD, "Passed paradigm != "
                  "SCOREP_PARADIGM_PTHREAD."  );

    int status = pthread_setspecific( tpd_key, NULL );
    UTILS_BUG_ON( status != 0, "Failed to reset Phread thread specific data." );

    /* currentTpd not needed anymore, maintain for reuse in the future */
    SCOREP_Location*      location   = scorep_thread_get_location( currentTpd );
    private_data_pthread* model_data = scorep_thread_get_model_data( currentTpd );
    size_t                reuse_key  = model_data->location_reuse_key;

    if ( reuse_key )
    {
        /* Returning the location into the thread pool identified by reuse_key */
        SCOREP_MutexLock( location_reuse_pool_mutex );

        size_t                      hash        = SCOREP_Hashtab_HashPointer( ( void* )reuse_key );
        size_t                      hash_bucket = hash & LOCATION_REUSE_POOL_MASK;
        struct location_reuse_pool* pool        = &location_reuse_pool[ hash_bucket ];

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
            pool = &location_reuse_pool[ hash_bucket ];
            if ( pool->reuse_key )
            {
                /* Really no pool yet, create a new one, taking memory
                 * from the just ended location
                 */
                pool = SCOREP_Location_AllocForMisc( location, sizeof( *pool ) );
                memset( pool, 0, sizeof( *pool ) );
                pool->next                              = location_reuse_pool[ hash_bucket ].next;
                location_reuse_pool[ hash_bucket ].next = pool;
            }

            /* We are occupying the pool now */
            pool->reuse_key = reuse_key;
        }

        /* We now have a pool, now we hook the location in the unused
         * locations list
         */
        struct reuse_pool_location* pool_location = location_reuse_pool_free_list;
        if ( pool_location )
        {
            location_reuse_pool_free_list = pool_location->next;
        }
        else
        {
            pool_location =
                SCOREP_Location_AllocForMisc( location,
                                              sizeof( *pool_location ) );
        }
        pool_location->location = location;
        pool_location->next     = pool->unused_locations;
        pool->unused_locations  = pool_location;

        SCOREP_MutexUnlock( location_reuse_pool_mutex );
    }

    UTILS_DEBUG_EXIT();
}


SCOREP_ParadigmType
scorep_thread_get_paradigm( void )
{
    return SCOREP_PARADIGM_PTHREAD;
}
