/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2016,
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
// IT is necessary to support rand_r()
#define _XOPEN_SOURCE

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static long            NUM_ITER        = 10;
static double*         buffer          = 0;
static int             buffer_is_empty = 1;
static pthread_mutex_t mutex           = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond_producer   = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  cond_consumer   = PTHREAD_COND_INITIALIZER;

void*
func_producer( void* input )
{
    long i;
    // initialize random seed
    int rand_seed = time( NULL );

    for ( i = 0; i < NUM_ITER; i++ )
    {
        pthread_mutex_lock( &mutex );

        while ( !buffer_is_empty )
        {
            pthread_cond_wait( &cond_producer, &mutex );
        }

        buffer  = malloc( sizeof( *buffer ) );
        *buffer = ( double )rand_r( &rand_seed ) / ( double )RAND_MAX;
//#ifdef DEBUG
        printf( "Iteration: %ld Producer produced: %g ", i, *buffer );
//#endif
        buffer_is_empty = 0;
        pthread_cond_signal( &cond_consumer );
        pthread_mutex_unlock( &mutex );
    }
    //pthread_exit( NULL );

    return NULL;
}

void*
func_consumer( void* input )
{
    long i;
    for ( i = 0; i < NUM_ITER; i++ )
    {
        pthread_mutex_lock( &mutex );

        while ( buffer_is_empty )
        {
            pthread_cond_wait( &cond_consumer, &mutex );
        }

//#ifdef DEBUG
        printf( "Iteration: %ld Consumer received: %g\n", i, *buffer );
//#endif
        *buffer = 0.0;
        free( buffer );
        buffer_is_empty = 1;
        pthread_cond_signal( &cond_producer );
        pthread_mutex_unlock( &mutex );
    }
    //pthread_exit( NULL );

    return NULL;
}

int
main( int argc, const char* argv[] )
{
    pthread_t producer, consumer;
    pthread_mutex_init( &mutex, NULL );
    pthread_cond_init( &cond_producer, NULL );
    pthread_cond_init( &cond_consumer, NULL );

    pthread_create( &producer, NULL, func_producer, NULL );
    pthread_create( &consumer, NULL, func_consumer, NULL );

    pthread_join( producer, NULL );
    pthread_join( consumer, NULL );

    pthread_mutex_destroy( &mutex );
    pthread_cond_destroy( &cond_producer );
    pthread_cond_destroy( &cond_consumer );

    return 0;
}
