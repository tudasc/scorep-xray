/*
** This file is part of the Score-P software (http://www.score-p.org)
**
** Copyright (c) 2014,
** Forschungszentrum Juelich GmbH, Germany
**
** This software may be modified and distributed under the terms of
** a BSD-style license.  See the COPYING file in the package base
** directory for details.
**
**/


/**
** @file
**/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

void
check_status( char* string, int status )
{
    if ( status )
    {
        printf( "%s test FAILED with error %d.\n", string, status );
        exit( 1 );
    }
#ifdef DEBUG
    printf( "%s test PASSED\n", string );
#endif
}


void
test_exit_child( void* arg )
{
    pthread_exit( arg );
}

void*
test_exit( void* arg )
{
    test_exit_child( arg );
    return NULL;
}


void
test_cancel_child()
{
    int i = 0;
    for (;; )
    {
        printf( "Thread is running, waiting to be cancelled: %d\n", i++ );
        pthread_testcancel();
        sleep( 1 );
    }
}

void*
test_cancel( void* arg )
{
    test_cancel_child();
    return NULL;
}


void*
test_detach( void* arg )
{
    sleep( 2 );
    return NULL;
}


int
main( int argc, char* argv[] )
{
    pthread_t      t;
    pthread_attr_t attr;
    int            state;
    int            status;

    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
    printf( "++++++++++++++++ Test of management routines +++++++++++++++\n" );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );

    status = pthread_attr_init( &attr );
    check_status( "pthread_attr_init()", status );


    /* pthread_exit  */
    status = pthread_create( &t, &attr, test_exit, NULL );
    check_status( "pthread_create()", status );
    sleep( 2 );
    printf( "Let's join the thread\n" );
    status = pthread_join( t, NULL );
    check_status( "pthread_join()", status );


    /* pthread_abort not supported yet */
    //status = pthread_create(&t, &attr, test_cancel, NULL);
    //check_status("pthread_create()", status)
    //sleep( 2 );
    //printf("Let's abort the thread!\n");
    //status = pthread_abort(t);
    //check_status("pthread_abort()", status);


    /* pthread_cancel */
    status = pthread_create( &t, &attr, test_cancel, NULL );
    check_status( "pthread_create()", status );
    sleep( 2 );
    printf( "Let's cancel the thread\n" );
    status = pthread_cancel( t );
    check_status( "pthread_cancel()", status );
    status = pthread_join( t, NULL );
    check_status( "pthread_join()", status );


    // test_detach needs to finish before we start finalization
    status = pthread_create( &t, &attr, test_detach, NULL );
    check_status( "pthread_create()", status );
    sleep( 1 );
    printf( "Let's detach the thread\n" );
    status = pthread_detach( t );
    check_status( "pthread_detach()", status );
    sleep( 2 );


    //printf( "Let's try to join detached thread\n" );
    //status = pthread_join( t, NULL );
    //if ( status == EINVAL )
    //{
    //    printf( "Thread is not joinable anymore\n", status );
    //}


    status = pthread_attr_destroy( &attr );
    check_status( "pthread_attr_destroy()", status );

    printf( "Test PASSED\n" );

    //pthread_exit( NULL );

    return 0;
}
