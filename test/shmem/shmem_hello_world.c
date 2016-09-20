/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 */


#include <stdio.h>
#include <shmem.h>


int
main( int    argc,
      char** argv )
{
    int   i, me, npes;
    int   start, stride, size;
    long* psync;

    start_pes( 0 );

    npes = _num_pes();
    me   = _my_pe();

    printf( "Hello from PE %d of %d\n", me + 1, npes );

    psync = shmalloc( sizeof( long ) * _SHMEM_BARRIER_SYNC_SIZE );
    for ( int i = 0; i < _SHMEM_BARRIER_SYNC_SIZE; ++i )
    {
        psync[ i ] = _SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();

    start  = 0;
    stride = 0;
    size   = npes;
    while ( me < size )
    {
        shmem_barrier( start, stride, size, psync );
        size /= 2;
    }

    shmem_barrier_all();

    shfree( psync );

    return 0;
}
