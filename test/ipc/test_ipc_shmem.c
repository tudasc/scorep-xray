/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 */

#include <config.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <shmem.h>

#ifdef SCOREP_LIBWRAP_STATIC

#define CALL_SHMEM( name ) __real_##name

#elif SCOREP_LIBWRAP_WEAK

#define CALL_SHMEM( name ) p##name

#else

#error Unsupported SHMEM wrapping mode

#endif

void
CALL_SHMEM( start_pes ) ( int );

int
CALL_SHMEM( _my_pe ) ( void );

int
CALL_SHMEM( _num_pes ) ( void );

void
CALL_SHMEM( shmem_barrier_all ) ( void );

#if HAVE( SHMEM_INT_MIN_TO_ALL_COMPLIANT )

void
CALL_SHMEM( shmem_int_min_to_all ) ( int*, int*, int, int, int, int, int*, long* );

#elif HAVE( SHMEM_INT_MIN_TO_ALL_CONST_VARIANT )

void
CALL_SHMEM( shmem_int_min_to_all ) ( int*, const int*, size_t, int, int, int, int*, long* );

#endif


#include "test_ipc.h"

static int  pes;
static long pSync[ _SHMEM_REDUCE_SYNC_SIZE ];
static int  pWrk[ _SHMEM_REDUCE_MIN_WRKDATA_SIZE ];
static int  reduce_args[ 2 ];


static void
reduce_results( int* result )
{
    reduce_args[ 0 ] = *result;

    CALL_SHMEM( shmem_barrier_all ) ();

    CALL_SHMEM( shmem_int_min_to_all ) ( &reduce_args[ 1 ],
                                         &reduce_args[ 0 ],
                                         1,
                                         0, 0, pes,
                                         pWrk, pSync );

    *result = reduce_args[ 1 ];

    CALL_SHMEM( shmem_barrier_all ) ();
}


int
main( int    argc,
      char** argv )
{
    int me;

    CALL_SHMEM( start_pes( 0 ) );
    pes = CALL_SHMEM( _num_pes ) ();
    me  = CALL_SHMEM( _my_pe ) ();

    for ( int i = 0; i < _SHMEM_REDUCE_SYNC_SIZE; i++ )
    {
        pSync[ i ] = _SHMEM_SYNC_VALUE;
    }
    CALL_SHMEM( shmem_barrier_all ) ();

    int ret = test_ipc( me, reduce_results );

    CALL_SHMEM( shmem_barrier_all ) ();

    /*
     * Brutally exit the program on failure.
     * Some stupid runtimes are calling _exit() in their atexit handler
     * but don't have the actually exit value at hand.
     */
    if ( EXIT_FAILURE == ret )
    {
        _exit( ret );
    }

    return ret;
}
