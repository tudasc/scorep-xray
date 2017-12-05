/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
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

#if HAVE( SHMEM_PROFILING_INTERFACE )
#define SHMEM_HAVE_DECL( DECL ) HAVE( SHMEM_SYMBOL_P ## DECL )
#elif HAVE( GNU_LINKER )
#define SHMEM_HAVE_DECL( DECL ) HAVE( SHMEM_SYMBOL_ ## DECL )
#endif

/* Enable stringize when 'name' is a macro. */
#define XCALL_SHMEM( name ) CALL_SHMEM( name )

#if SHMEM_HAVE_DECL( SHMEM_N_PES )
#define SCOREP_SHMEM_N_PES shmem_n_pes
#else
#define SCOREP_SHMEM_N_PES _num_pes
#endif

#if SHMEM_HAVE_DECL( SHMEM_MY_PE )
#define SCOREP_SHMEM_MY_PE shmem_my_pe
#else
#define SCOREP_SHMEM_MY_PE _my_pe
#endif

void
CALL_SHMEM( start_pes )( int );

int
XCALL_SHMEM( SCOREP_SHMEM_MY_PE )( void );

int
XCALL_SHMEM( SCOREP_SHMEM_N_PES )( void );

void
CALL_SHMEM( shmem_barrier_all )( void );

#if defined( SCOREP_SHMEM_INT_MIN_TO_ALL_PROTO_ARGS )

/* *INDENT-OFF* */
void
CALL_SHMEM( shmem_int_min_to_all ) SCOREP_SHMEM_INT_MIN_TO_ALL_PROTO_ARGS;
/* *INDENT-ON* */

#else

#error No suitable reduction function available!

#endif


#include "test_ipc.h"

static int pes;
static long pSync[ _SHMEM_REDUCE_SYNC_SIZE ];
static int  pWrk[ _SHMEM_REDUCE_MIN_WRKDATA_SIZE ];
static int  reduce_args[ 2 ];


static void
reduce_results( int* result )
{
    reduce_args[ 0 ] = *result;

    CALL_SHMEM( shmem_barrier_all )();

    CALL_SHMEM( shmem_int_min_to_all )( &reduce_args[ 1 ],
                                        &reduce_args[ 0 ],
                                        1,
                                        0, 0, pes,
                                        pWrk, pSync );

    *result = reduce_args[ 1 ];

    CALL_SHMEM( shmem_barrier_all )();
}


int
main( int    argc,
      char** argv )
{
    int me;

    CALL_SHMEM( start_pes )( 0 );
    pes = XCALL_SHMEM( SCOREP_SHMEM_N_PES )();
    me  = XCALL_SHMEM( SCOREP_SHMEM_MY_PE )();

    for ( int i = 0; i < _SHMEM_REDUCE_SYNC_SIZE; i++ )
    {
        pSync[ i ] = _SHMEM_SYNC_VALUE;
    }
    CALL_SHMEM( shmem_barrier_all )();

    int ret = test_ipc( me, reduce_results );

    CALL_SHMEM( shmem_barrier_all )();

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
