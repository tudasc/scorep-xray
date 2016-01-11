/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Implementation of internal functins for lock management.
 */

#include <config.h>
#include <stdlib.h>

#include <SCOREP_Definitions.h>

#include "SCOREP_Opari2_Openmp_Lock.h"

#define SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE 100

struct lock_block
{
    struct SCOREP_Opari2_Openmp_Lock lock[ SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE ];
    struct lock_block*               next;
    struct lock_block*               prev;
};

static struct lock_block*                lock_head_block = 0;
static struct lock_block*                lock_last_block = 0;
static struct SCOREP_Opari2_Openmp_Lock* last_lock       = 0;
static int                               last_index      = SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE;

static SCOREP_Opari2_Openmp_Lock_HandleType current_lock_handle = 0;

SCOREP_Mutex scorep_opari2_openmp_lock = SCOREP_INVALID_MUTEX;

void
scorep_opari2_openmp_lock_initialize( void )
{
    SCOREP_MutexCreate( &scorep_opari2_openmp_lock );
}

void
scorep_opari2_openmp_lock_finalize( void )
{
    struct lock_block* block;

    /* free lock blocks */

    while ( lock_head_block )
    {
        block           = lock_head_block;
        lock_head_block = lock_head_block->next;
        free( block );
    }

    SCOREP_MutexDestroy( &scorep_opari2_openmp_lock );
}

SCOREP_Opari2_Openmp_Lock*
scorep_opari2_openmp_lock_init( const void* lock )
{
    struct lock_block* new_block;

    SCOREP_MutexLock( scorep_opari2_openmp_lock );
    last_index++;
    if ( last_index >= SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE )
    {
        if ( lock_head_block == 0 )
        {
            /* first time: allocate and initialize first block */
            new_block       = malloc( sizeof( struct lock_block ) );
            new_block->next = 0;
            new_block->prev = 0;
            lock_head_block = lock_last_block = new_block;
        }
        else if ( lock_last_block == 0 )
        {
            /* lock list empty: re-initialize */
            lock_last_block = lock_head_block;
        }
        else
        {
            if ( lock_last_block->next == 0 )
            {
                /* lock list full: expand */
                new_block             = malloc( sizeof( struct lock_block ) );
                new_block->next       = 0;
                new_block->prev       = lock_last_block;
                lock_last_block->next = new_block;
            }
            /* use next available block */
            lock_last_block = lock_last_block->next;
        }
        last_lock  = &( lock_last_block->lock[ 0 ] );
        last_index = 0;
    }
    else
    {
        last_lock++;
    }
    /* store lock information */
    last_lock->lock              = lock;
    last_lock->handle            = current_lock_handle++;
    last_lock->acquisition_order = 0;
    last_lock->nest_level        = 0;

    SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
    return last_lock;
}

static struct SCOREP_Opari2_Openmp_Lock*
scorep_opari2_openmp_get_lock( const void* lock )
{
    int                               i;
    struct lock_block*                block;
    struct SCOREP_Opari2_Openmp_Lock* curr;

    /* search all locks in all blocks */
    block = lock_head_block;
    while ( block )
    {
        curr = &( block->lock[ 0 ] );
        for ( i = 0; i < SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE; ++i )
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


SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetAcquireLock( const void* lock )
{
    SCOREP_Opari2_Openmp_Lock* lock_struct = scorep_opari2_openmp_get_lock( lock );
    lock_struct->acquisition_order++;
    return lock_struct;
}


SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetReleaseLock( const void* lock )
{
    return scorep_opari2_openmp_get_lock( lock );
}


SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetAcquireNestLock( const void* lock )
{
    SCOREP_Opari2_Openmp_Lock* lock_struct = scorep_opari2_openmp_get_lock( lock );
    if ( lock_struct->nest_level == 0 )
    {
        lock_struct->acquisition_order++;
    }
    lock_struct->nest_level++;
    return lock_struct;
}


SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetReleaseNestLock( const void* lock )
{
    SCOREP_Opari2_Openmp_Lock* lock_struct = scorep_opari2_openmp_get_lock( lock );
    lock_struct->nest_level--;
    return lock_struct;
}


SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetLock( const void* lock )
{
    // duplication, I (CR) know. The entire lock handling needs cleanup.
    // I will create a ticket for that.
    return scorep_opari2_openmp_get_lock( lock );
}


void
scorep_opari2_openmp_lock_destroy( const void* lock )
{
    /* delete lock by copying last lock in place of lock */

    *scorep_opari2_openmp_get_lock( lock ) = *last_lock;

    /* adjust pointer to last lock  */
    last_index--;
    if ( last_index < 0 )
    {
        /* reached low end of block */
        if ( lock_last_block->prev )
        {
            /* goto previous block if existing */
            last_index = SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE - 1;
            last_lock  = &( lock_last_block->
                            prev->lock[ last_index ] );
        }
        else
        {
            /* no previous block: re-initialize */
            last_index = SCOREP_OPARI2_OPENMP_LOCKBLOCK_SIZE;
            last_lock  = 0;
        }
        lock_last_block = lock_last_block->prev;
    }
    else
    {
        last_lock--;
    }
}
