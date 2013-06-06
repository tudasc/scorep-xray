/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Pomp_Lock.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP
 *
 * @brief Implementation of internal functins for lock management.
 */

#include <config.h>
#include <stdlib.h>

#include "SCOREP_Pomp_Lock.h"
#include "SCOREP_Definitions.h"

#define SCOREP_POMP_LOCKBLOCK_SIZE 100

struct scorep_pomp_lock_block
{
    struct SCOREP_PompLock         lock[ SCOREP_POMP_LOCKBLOCK_SIZE ];
    struct scorep_pomp_lock_block* next;
    struct scorep_pomp_lock_block* prev;
};

static struct scorep_pomp_lock_block* scorep_pomp_lock_head_block = 0;
static struct scorep_pomp_lock_block* scorep_pomp_lock_last_block = 0;
static struct SCOREP_PompLock*        scorep_pomp_last_lock       = 0;
static int                            scorep_pomp_last_index      = SCOREP_POMP_LOCKBLOCK_SIZE;

static SCOREP_Pomp_LockHandleType scorep_pomp_current_lock_handle = 0;

SCOREP_Mutex scorep_pomp_lock_lock;

void
scorep_pomp_lock_initialize( void )
{
    SCOREP_MutexCreate( &scorep_pomp_lock_lock );
}

void
scorep_pomp_lock_close( void )
{
    struct scorep_pomp_lock_block* block;

    /* free lock blocks */

    while ( scorep_pomp_lock_head_block )
    {
        block                       = scorep_pomp_lock_head_block;
        scorep_pomp_lock_head_block = scorep_pomp_lock_head_block->next;
        free( block );
    }

    SCOREP_MutexDestroy( &scorep_pomp_lock_lock );
}

SCOREP_PompLock*
scorep_pomp_lock_init( const void* lock )
{
    struct scorep_pomp_lock_block* new_block;

    SCOREP_MutexLock( scorep_pomp_lock_lock );
    scorep_pomp_last_index++;
    if ( scorep_pomp_last_index >= SCOREP_POMP_LOCKBLOCK_SIZE )
    {
        if ( scorep_pomp_lock_head_block == 0 )
        {
            /* first time: allocate and initialize first block */
            new_block                   = malloc( sizeof( struct scorep_pomp_lock_block ) );
            new_block->next             = 0;
            new_block->prev             = 0;
            scorep_pomp_lock_head_block = scorep_pomp_lock_last_block = new_block;
        }
        else if ( scorep_pomp_lock_last_block == 0 )
        {
            /* lock list empty: re-initialize */
            scorep_pomp_lock_last_block = scorep_pomp_lock_head_block;
        }
        else
        {
            if ( scorep_pomp_lock_last_block->next == 0 )
            {
                /* lock list full: expand */
                new_block                         = malloc( sizeof( struct scorep_pomp_lock_block ) );
                new_block->next                   = 0;
                new_block->prev                   = scorep_pomp_lock_last_block;
                scorep_pomp_lock_last_block->next = new_block;
            }
            /* use next available block */
            scorep_pomp_lock_last_block = scorep_pomp_lock_last_block->next;
        }
        scorep_pomp_last_lock  = &( scorep_pomp_lock_last_block->lock[ 0 ] );
        scorep_pomp_last_index = 0;
    }
    else
    {
        scorep_pomp_last_lock++;
    }
    /* store lock information */
    scorep_pomp_last_lock->lock              = lock;
    scorep_pomp_last_lock->handle            = scorep_pomp_current_lock_handle++;
    scorep_pomp_last_lock->acquisition_order = 0;
    scorep_pomp_last_lock->nest_level        = 0;

    SCOREP_MutexUnlock( scorep_pomp_lock_lock );
    return scorep_pomp_last_lock;
}

static struct SCOREP_PompLock*
scorep_pomp_get_lock( const void* lock )
{
    int                            i;
    struct scorep_pomp_lock_block* block;
    struct SCOREP_PompLock*        curr;

    /* search all locks in all blocks */
    block = scorep_pomp_lock_head_block;
    while ( block )
    {
        curr = &( block->lock[ 0 ] );
        for ( i = 0; i < SCOREP_POMP_LOCKBLOCK_SIZE; ++i )
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


SCOREP_PompLock*
SCOREP_Pomp_GetAcquireLock( const void* lock )
{
    SCOREP_PompLock* lock_struct = scorep_pomp_get_lock( lock );
    lock_struct->acquisition_order++;
    return lock_struct;
}


SCOREP_PompLock*
SCOREP_Pomp_GetReleaseLock( const void* lock )
{
    return scorep_pomp_get_lock( lock );
}


SCOREP_PompLock*
SCOREP_Pomp_GetAcquireNestLock( const void* lock )
{
    SCOREP_PompLock* lock_struct = scorep_pomp_get_lock( lock );
    if ( lock_struct->nest_level == 0 )
    {
        lock_struct->acquisition_order++;
    }
    lock_struct->nest_level++;
    return lock_struct;
}


SCOREP_PompLock*
SCOREP_Pomp_GetReleaseNestLock( const void* lock )
{
    SCOREP_PompLock* lock_struct = scorep_pomp_get_lock( lock );
    lock_struct->nest_level--;
    return lock_struct;
}


SCOREP_PompLock*
SCOREP_Pomp_GetLock( const void* lock )
{
    // duplication, I (CR) know. The entire lock handling needs cleanup.
    // I will create a ticket for that.
    return scorep_pomp_get_lock( lock );
}


void
scorep_pomp_lock_destroy( const void* lock )
{
    /* delete lock by copying last lock in place of lock */

    *scorep_pomp_get_lock( lock ) = *scorep_pomp_last_lock;

    /* adjust pointer to last lock  */
    scorep_pomp_last_index--;
    if ( scorep_pomp_last_index < 0 )
    {
        /* reached low end of block */
        if ( scorep_pomp_lock_last_block->prev )
        {
            /* goto previous block if existing */
            scorep_pomp_last_index = SCOREP_POMP_LOCKBLOCK_SIZE - 1;
            scorep_pomp_last_lock  = &( scorep_pomp_lock_last_block->
                                        prev->lock[ scorep_pomp_last_index ] );
        }
        else
        {
            /* no previous block: re-initialize */
            scorep_pomp_last_index = SCOREP_POMP_LOCKBLOCK_SIZE;
            scorep_pomp_last_lock  = 0;
        }
        scorep_pomp_lock_last_block = scorep_pomp_lock_last_block->prev;
    }
    else
    {
        scorep_pomp_last_lock--;
    }
}
