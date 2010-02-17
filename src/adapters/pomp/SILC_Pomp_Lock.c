/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Pomp_Lock.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Implementation of internal functins for lock management.
 */

#include <stdlib.h>

#include "SILC_Pomp_Lock.h"
#include "SILC_Definitions.h"

extern SILC_SourceFileHandle silc_pomp_file_handle;

struct silc_pomp_lock
{
    const void*              lock;
    SILC_Pomp_LockHandleType handle;
};

#define SILC_POMP_LOCKBLOCK_SIZE 100

struct silc_pomp_lock_block
{
    struct silc_pomp_lock        lock[ SILC_POMP_LOCKBLOCK_SIZE ];
    struct silc_pomp_lock_block* next;
    struct silc_pomp_lock_block* prev;
};

static struct silc_pomp_lock_block* silc_pomp_lock_head_block = 0;
static struct silc_pomp_lock_block* silc_pomp_lock_last_block = 0;
static struct silc_pomp_lock*       silc_pomp_last_lock       = 0;
static int                          silc_pomp_last_index      = SILC_POMP_LOCKBLOCK_SIZE;

static SILC_Pomp_LockHandleType     silc_pomp_current_lock_handle = 0;

/** List of registered omp function names. They must be in the same order as the
    corresponding SILC_Pomp_Region_Index.
 */
char* silc_pomp_region_names[] =
{
    "omp_init_lock",
    "omp_destroy_lock",
    "omp_set_lock",
    "omp_unset_lock",
    "omp_test_lock",
    "omp_init_nest_lock",
    "omp_destroy_nest_lock",
    "omp_set_nest_lock",
    "omp_unset_nest_lock",
    "omp_test_nest_lock"
};

void
silc_pomp_register_lock_regions()
{
    int i = 0;
    for ( i = 0; i < SILC_POMP_REGION_NUM; i++ )
    {
        silc_pomp_regid[ i ] = SILC_DefineRegion( silc_pomp_region_names[ i ],
                                                  silc_pomp_file_handle,
                                                  SILC_INVALID_LINE_NO,
                                                  SILC_INVALID_LINE_NO,
                                                  SILC_ADAPTER_POMP,
                                                  SILC_REGION_OMP_WRAPPER );
    }
}

void
silc_pomp_lock_close()
{
    struct silc_pomp_lock_block* block;

    /* free lock blocks */

    while ( silc_pomp_lock_head_block )
    {
        block                     = silc_pomp_lock_head_block;
        silc_pomp_lock_head_block = silc_pomp_lock_head_block->next;
        free( block );
    }
}

SILC_Pomp_LockHandleType
silc_pomp_lock_init( const void* lock )
{
    struct silc_pomp_lock_block* new_block;

    silc_pomp_last_index++;
    if ( silc_pomp_last_index >= SILC_POMP_LOCKBLOCK_SIZE )
    {
        if ( silc_pomp_lock_head_block == 0 )
        {
            /* first time: allocate and initialize first block */
            new_block                 = malloc( sizeof( struct silc_pomp_lock_block ) );
            new_block->next           = 0;
            new_block->prev           = 0;
            silc_pomp_lock_head_block = silc_pomp_lock_last_block = new_block;
        }
        else if ( silc_pomp_lock_last_block == 0 )
        {
            /* lock list empty: re-initialize */
            silc_pomp_lock_last_block = silc_pomp_lock_head_block;
        }
        else
        {
            if ( silc_pomp_lock_last_block->next == 0 )
            {
                /* lock list full: expand */
                new_block                       = malloc( sizeof( struct silc_pomp_lock_block ) );
                new_block->next                 = 0;
                new_block->prev                 = silc_pomp_lock_last_block;
                silc_pomp_lock_last_block->next = new_block;
            }
            /* use next available block */
            silc_pomp_lock_last_block = silc_pomp_lock_last_block->next;
        }
        silc_pomp_last_lock  = &( silc_pomp_lock_last_block->lock[ 0 ] );
        silc_pomp_last_index = 0;
    }
    else
    {
        silc_pomp_last_lock++;
    }
    /* store lock information */
    silc_pomp_last_lock->lock   = lock;
    silc_pomp_last_lock->handle = silc_pomp_current_lock_handle++;
    return silc_pomp_last_lock->handle;
}

static struct silc_pomp_lock*
silc_pomp_get_lock( const void* lock )
{
    int                          i;
    struct silc_pomp_lock_block* block;
    struct silc_pomp_lock*       curr;

    /* search all locks in all blocks */
    block = silc_pomp_lock_head_block;
    while ( block )
    {
        curr = &( block->lock[ 0 ] );
        for ( i = 0; i < SILC_POMP_LOCKBLOCK_SIZE; ++i )
        {
            if ( curr->lock == lock )
            {
                return curr;
            }

            curr++;
        }
        block = block->next;
    }
    return 0;
}

/** Returns the lock handle pair for a given OMP lock. */
SILC_Pomp_LockHandleType
silc_pomp_get_lock_handle( const void* lock )
{
    return silc_pomp_get_lock( lock )->handle;
}

void
silc_pomp_lock_destroy( const void* lock )
{
    /* delete lock by copying last lock in place of lock */

    *silc_pomp_get_lock( lock ) = *silc_pomp_last_lock;

    /* adjust pointer to last lock  */
    silc_pomp_last_index--;
    if ( silc_pomp_last_index < 0 )
    {
        /* reached low end of block */
        if ( silc_pomp_lock_last_block->prev )
        {
            /* goto previous block if existing */
            silc_pomp_last_index = SILC_POMP_LOCKBLOCK_SIZE - 1;
            silc_pomp_last_lock  = &( silc_pomp_lock_last_block->
                                      prev->lock[ silc_pomp_last_index ] );
        }
        else
        {
            /* no previous block: re-initialize */
            silc_pomp_last_index = SILC_POMP_LOCKBLOCK_SIZE;
            silc_pomp_last_lock  = 0;
        }
        silc_pomp_lock_last_block = silc_pomp_lock_last_block->prev;
    }
    else
    {
        silc_pomp_last_lock--;
    }
}
