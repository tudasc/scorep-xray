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
** @file
**/

/* usleep and rand_r are non-portable and removed / marked obsolete in recent
 * POSIX versions. Both are defined by BSD, but _BSD_SOURCE is deprecated
 * Fortunately, both are specified by SUSv2 */
#define _XOPEN_SOURCE 500

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/errno.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5
#define SPAGETTI 10

typedef struct philosoph
{
    double think_time;
    double eat_time;
    int    left_fork;
    int    right_fork;
    long   thread_id;
    time_t total_thinking_time;
    time_t total_eating_time;
    time_t total_starvation_time;
    long   spagetti;
} philosoph_t;

static pthread_mutex_t forks[ NUM_PHILOSOPHERS ];
static philosoph_t     ph[ NUM_PHILOSOPHERS ];

time_t
think( double think_time, long thread_id )
{
    time_t timer = time( NULL );
#ifdef DEBUG
    printf( "Philosopher %ld is thinking\n", thread_id );
#endif
    usleep( think_time );
    return time( NULL ) - timer;
}

time_t
eat( double eat_time, long thread_id, long i )
{
    time_t timer = time( NULL );
#ifdef DEBUG
    printf( "Philosopher %ld is eating %ld out of %ld\n",
            thread_id, SPAGETTI - i, SPAGETTI );
#endif
    usleep( eat_time );
    return time( NULL ) - timer;
}

void*
dining( void* input )
{
    time_t       timer      = 0;
    philosoph_t* ph         = ( philosoph_t* )input;
    unsigned int think_seed = time( NULL ) + ph->thread_id;
    unsigned int eat_seed   = time( NULL ) + ph->thread_id + 1;

    ph->think_time = 1000000 * ( double )rand_r( &think_seed ) / ( double )RAND_MAX;
    ph->eat_time   = 1000000 * ( double )rand_r( &eat_seed ) / ( double )RAND_MAX;

#ifdef DEBUG
    printf( "I'm philosopher #%ld my left fork is #%ld my right fork is #%ld my "
            "thinking time is %g ms my eating time is %g ms\n", ph->thread_id,
            ph->left_fork, ph->right_fork, ph->think_time, ph->eat_time );
#endif
    while ( ph->spagetti != 0 )
    {
        bool two_forks_avail = false;
        do
        {
            if ( pthread_mutex_lock( &forks[ ph->left_fork ] ) != 0 )
            {
                printf( "ERROR: Left lock can not be locked\n" );
            }
            int right_fork_result;

            right_fork_result = pthread_mutex_trylock( &forks[ ph->right_fork ] );
            switch ( right_fork_result )
            {
                case 0:
                    two_forks_avail = true;
                    break;
                case EBUSY:
                    timer = time( NULL );
                    pthread_mutex_unlock( &forks[ ph->left_fork ] );
                    two_forks_avail = false;
                    break;
                default:
                    printf( "Right lock can not be locked!\n" );
            }
        }
        while ( !two_forks_avail );

        if ( time( NULL ) > timer && timer != 0 )
        {
            ph->total_starvation_time += time( NULL ) - timer;
            timer                      = 0;
        }
        ph->spagetti           = ph->spagetti - 1;
        ph->total_eating_time += eat( ph->eat_time, ph->thread_id, ph->spagetti );
        pthread_mutex_unlock( &forks[ ph->right_fork ] );
        pthread_mutex_unlock( &forks[ ph->left_fork ] );
        ph->total_thinking_time += think( ph->think_time, ph->thread_id );
    }
    printf( "Philosopher %ld total eating time: %ld total thinking time: %ld "
            "starvation: %ld\n", ph->thread_id, ph->total_eating_time,
            ph->total_thinking_time, ph->total_starvation_time );
    return NULL;
}

int
main( int argc, const char* argv[] )
{
    int       i;
    pthread_t t[ NUM_PHILOSOPHERS ];

    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
        if ( ( pthread_mutex_init( &forks[ i ], NULL ) ) != 0 )
        {
            printf( "ERROR: pthread_mutex_init failed\n" );
        }
    }

    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
        ph[ i ].think_time            = 0.0;
        ph[ i ].eat_time              = 0.0;
        ph[ i ].left_fork             = i;
        ph[ i ].right_fork            = i + 1;
        ph[ i ].thread_id             = i;
        ph[ i ].total_thinking_time   = 0;
        ph[ i ].total_eating_time     = 0;
        ph[ i ].total_starvation_time = 0;
        ph[ i ].spagetti              = SPAGETTI;
    }

    // correct first and last values
    ph[ NUM_PHILOSOPHERS - 1 ].right_fork = 0;

    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
        if ( ( pthread_create( &t[ i ], NULL, dining, ( void* )&ph[ i ] ) ) != 0 )
        {
            printf( "ERROR: pthread_create failed\n" );
        }
    }

    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
        if ( ( pthread_join( t[ i ], NULL ) ) != 0 )
        {
            printf( "ERROR: pthread_join failed\n" );
        }
    }

    return 0;
}
