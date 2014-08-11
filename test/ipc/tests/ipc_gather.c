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

#include <CuTest.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "ipc_test_utils.h"
#include "ipc_tests.h"

#include <scorep_ipc.h>

void
test_ipc_gather( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();

    int number_of_elements = 2;
    int data_to_send[ 2 ]  =
    {
        my_rank,
        my_rank + 1
    };

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        int* recived_data  = NULL;
        int* expected_data = NULL;

        if ( root == my_rank )
        {
            recived_data  = calloc( number_of_elements * number_of_ranks, sizeof( int ) );
            expected_data = calloc( number_of_elements * number_of_ranks, sizeof( int ) );
            for ( int i = 0; i < number_of_ranks; ++i )
            {
                expected_data[ ( i * number_of_elements ) ]     = i;
                expected_data[ ( i * number_of_elements ) + 1 ] = i + 1;
            }
        }

        int return_value = SCOREP_Ipc_Gather( data_to_send,
                                              recived_data,
                                              number_of_elements,
                                              SCOREP_IPC_INT,
                                              root );

        DO_ASSERTIONS( 1,
                       number_of_elements * number_of_ranks )

        free( expected_data );
        free( recived_data );
    }
} /* test_ipc_gather */


void
test_ipc_gatherv( CuTest* tc )
{
    int my_rank                   = SCOREP_Ipc_GetRank();
    int number_of_ranks           = SCOREP_Ipc_GetSize();
    int number_of_elements        = my_rank + 1;
    int number_of_elements_recive = CALCULATE_TRIANGULAR_NUMBER( number_of_ranks );

    /*
     * P0: {0}
     * P1: {1,1}
     * P2: {2,2,2}
     */
    int* data_to_send = calloc( number_of_elements, sizeof( int ) );
    for ( int i = 0; i < number_of_elements; ++i )
    {
        data_to_send[ i ] = my_rank;
    }

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        int* recived_data  = NULL;
        int* expected_data = NULL;
        int* counts        = NULL;


        if ( root == my_rank )
        {
            recived_data  = calloc( number_of_elements_recive, sizeof( int ) );
            expected_data = calloc( number_of_elements_recive, sizeof( int ) );
            counts        = calloc( number_of_ranks, sizeof( int ) );

            int index = 0;
            for ( int i = 0; i < number_of_ranks; ++i )
            {
                counts[ i ] = i + 1;
                for ( int j = 0; j <= i; ++j )
                {
                    expected_data[ index++ ] = i;
                }
            }
        }

        int return_value = SCOREP_Ipc_Gatherv( data_to_send,
                                               number_of_elements,
                                               recived_data,
                                               counts,
                                               SCOREP_IPC_INT,
                                               root );

        DO_ASSERTIONS( 1,
                       number_of_elements_recive )

        free( recived_data );
        free( expected_data );
        free( counts );
    }

    free( data_to_send );
} /* test_ipc_gatherv */


void
test_ipc_gatherv0( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();
    int data_to_send    = my_rank;

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        int  number_of_elements = 0;
        int* recived_data       = NULL;
        int* expected_data      = NULL;
        int* counts             = NULL;

        if ( root == my_rank )
        {
            recived_data       = calloc( 1, sizeof( int ) );
            expected_data      = calloc( 1, sizeof( int ) );
            expected_data[ 0 ] = root;
            counts             = calloc( number_of_ranks, sizeof( int ) );
            counts[ root ]     = 1;
            number_of_elements = 1;
        }

        int return_value = SCOREP_Ipc_Gatherv( &data_to_send,
                                               number_of_elements,
                                               recived_data,
                                               counts,
                                               SCOREP_IPC_INT,
                                               root );

        DO_ASSERTIONS( 1, 1 )

        free( recived_data );
        free( expected_data );
        free( counts );
    }
} /* test_ipc_gatherv0 */


void
test_ipc_allgather( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();

    int  data_to_send  = my_rank;
    int* recived_data  = calloc( number_of_ranks, sizeof( int ) );
    int* expected_data = calloc( number_of_ranks, sizeof( int ) );
    for ( int i = 0; i < number_of_ranks; ++i )
    {
        expected_data[ i ] = i;
    }

    int return_value = SCOREP_Ipc_Allgather( &data_to_send,
                                             recived_data,
                                             1,
                                             SCOREP_IPC_INT );

    DO_ASSERTIONS( 1,
                   number_of_ranks )

    free( recived_data );
    free( expected_data );
} /* test_ipc_allgather */
