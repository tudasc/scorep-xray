/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2019,
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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define M_PI 3.14159265358979323846

typedef struct pthread_input
{
    long number_of_points;
    long hits_to_circle_area;
    long thread_id;
} pthread_input_t;

void*
number_of_hits_per_thread( void* input )
{
    pthread_input_t* pt_input = ( pthread_input_t* )input;

    double x, y;
    long   i;
    // initialize random seed
    unsigned rand_seed_x = time( NULL ) + pt_input->thread_id;
    unsigned rand_seed_y = time( NULL ) + pt_input->thread_id + 1;

    for ( i = 0; i < pt_input->number_of_points; i++ )
    {
        x = ( double )rand_r( &rand_seed_x ) / ( double )RAND_MAX;
        y = ( double )rand_r( &rand_seed_y ) / ( double )RAND_MAX;

        if ( ( x * x + y * y ) <= 1 )
        {
            pt_input->hits_to_circle_area++;
        }

#ifdef DEBUG
        printf( "Thread id: %ld point %ld out of %ld x: %g y: %g #hits: %ld\n",
                pt_input->thread_id, i + 1, pt_input->number_of_points, x, y,
                pt_input->hits_to_circle_area );
#endif
    }
    return NULL;
}


int
main( int argc, const char* argv[] )
{
    long number_of_points  = 1000;
    long number_of_threads = 4;

    if ( argc != 3 )
    {
        printf( "Usage: ./pi-pthread-cc <number_of_points> <number_of_threads>\n"
                "       using default values (1000, 4).\n" );
    }
    else
    {
        number_of_points  = atol( argv[ 1 ] );
        number_of_threads = atol( argv[ 2 ] );
    }

    long            points_per_thread = number_of_points / number_of_threads;
    pthread_input_t pthread_input[ number_of_threads ];

    long    i;
    double  pi;
    clock_t timer;
    long    hits_to_circle_area = 0;

    pthread_t threads[ number_of_threads ];

    // initialize initial values for threads
    for ( i = 0; i < number_of_threads; i++ )
    {
        pthread_input[ i ].number_of_points    = points_per_thread;
        pthread_input[ i ].hits_to_circle_area = 0;
        pthread_input[ i ].thread_id           = i;
    }

    if ( number_of_points % number_of_threads != 0 )
    {
        pthread_input[ number_of_threads - 1 ].number_of_points =
            points_per_thread + number_of_points % number_of_threads;
    }

    timer = clock();

    for ( i = 0; i < number_of_threads; i++ )
    {
        pthread_create( &threads[ i ],
                        NULL,
                        number_of_hits_per_thread,
                        ( void* )&pthread_input[ i ] );
    }

    for ( i = 0; i < number_of_threads; i++ )
    {
        pthread_join( threads[ i ], NULL );
        hits_to_circle_area += pthread_input[ i ].hits_to_circle_area;
    }

    pi    = ( double )4.0 * hits_to_circle_area / number_of_points;
    timer = clock() - timer;

    printf( "Approximate value of pi with %lu iterations is: %g\n", number_of_points, pi );
    printf( "Difference between exact and computed values is: %g\n", pi - M_PI );
    printf( "Error: %f%%\n", ( float )( fabs( ( pi - M_PI ) / ( M_PI ) ) * 100 ) );
    printf( "Computation time: %fs\n", ( ( float )timer ) / CLOCKS_PER_SEC );

    return 0;
}
