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
test_ipc_send_recive_one_to_one( CuTest* tc )
{
    int my_rank            = SCOREP_Ipc_GetRank();
    int number_of_ranks    = SCOREP_Ipc_GetSize();
    int return_value       = -1;
    int number_of_elements = 10;

    int* expected_data = NULL;
    int* recived_data  = NULL;

    if ( my_rank == 0 )
    {
        int data_to_send[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        return_value = SCOREP_Ipc_Send( data_to_send,
                                        number_of_elements,
                                        SCOREP_IPC_INT,
                                        number_of_ranks - 1 );
    }
    else if ( my_rank == number_of_ranks - 1 )
    {
        recived_data  = calloc( number_of_elements, sizeof( int ) );
        expected_data = calloc( number_of_elements, sizeof( int ) );
        for ( int i = 0; i < number_of_elements; ++i )
        {
            expected_data[ i ] = i + 1;
        }

        return_value = SCOREP_Ipc_Recv( recived_data,
                                        number_of_elements,
                                        SCOREP_IPC_INT,
                                        0 );
    }

    DO_ASSERTIONS( my_rank == 0
                   || my_rank == number_of_ranks - 1,
                   number_of_elements )

    free( expected_data );
    free( recived_data );
} /* test_ipc_send_recive_one_to_one */


void
test_ipc_send_recive_all_to_one( CuTest* tc )
{
    int my_rank            = SCOREP_Ipc_GetRank();
    int number_of_ranks    = SCOREP_Ipc_GetSize();
    int return_value       = -1;
    int number_of_elements = 10;

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        for ( int remote = 0; remote < number_of_ranks; ++remote )
        {
            int* recived_data  = NULL;
            int* expected_data = NULL;

            if ( remote == root )
            {
                continue;
            }

            if ( root == my_rank )
            {
                recived_data  = calloc( number_of_elements, sizeof( int ) );
                expected_data = calloc( number_of_elements, sizeof( int ) );

                for ( int j = 0; j < number_of_elements; ++j )
                {
                    expected_data[ j ] = j + remote + 1;
                }

                return_value = SCOREP_Ipc_Recv( recived_data,
                                                number_of_elements,
                                                SCOREP_IPC_INT,
                                                remote );
            }
            else if ( remote == my_rank )
            {
                /*
                 * P0: { 1, 2, 3, 4, 5, 6, 7, 8, 9,10}
                 * P1: { 2, 3, 4, 5, 6, 7, 8, 9,10,11}
                 * P2: { 3, 4, 5, 6, 7, 8, 9,10,11,12}
                 */
                int* data_to_send = calloc( number_of_elements, sizeof( int ) );
                for ( int i = 0; i < number_of_elements; ++i )
                {
                    data_to_send[ i ] = i + my_rank + 1;
                }

                return_value = SCOREP_Ipc_Send( data_to_send,
                                                number_of_elements,
                                                SCOREP_IPC_INT,
                                                root );

                free( data_to_send );
            }

            DO_ASSERTIONS( root == my_rank
                           || remote == my_rank,
                           number_of_elements )

            free( recived_data );
            free( expected_data );
        }
    }
} /* test_ipc_send_recive_all_to_one */


void
test_ipc_send_recive_one_to_all( CuTest* tc )
{
    int my_rank            = SCOREP_Ipc_GetRank();
    int number_of_ranks    = SCOREP_Ipc_GetSize();
    int return_value       = -1;
    int number_of_elements = 10;

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        for ( int remote = 0; remote < number_of_ranks; ++remote )
        {
            int* recived_data  = NULL;
            int* expected_data = NULL;

            if ( remote == root )
            {
                continue;
            }

            if ( root == my_rank )
            {
                /*
                 * P0: { 1, 2, 3, 4, 5, 6, 7, 8, 9,10}
                 * P1: { 2, 3, 4, 5, 6, 7, 8, 9,10,11}
                 * P2: { 3, 4, 5, 6, 7, 8, 9,10,11,12}
                 */
                int* data_to_send = calloc( number_of_elements, sizeof( int ) );
                for ( int j = 0; j < number_of_elements; ++j )
                {
                    data_to_send[ j ] = j + remote + 1;
                }

                return_value = SCOREP_Ipc_Send( data_to_send,
                                                number_of_elements,
                                                SCOREP_IPC_INT,
                                                remote );
                free( data_to_send );
            }
            else if ( remote == my_rank )
            {
                recived_data  = calloc( number_of_elements, sizeof( int ) );
                expected_data = calloc( number_of_elements, sizeof( int ) );
                for ( int i = 0; i < number_of_elements; ++i )
                {
                    expected_data[ i ] = i + my_rank + 1;
                }

                return_value = SCOREP_Ipc_Recv( recived_data,
                                                number_of_elements,
                                                SCOREP_IPC_INT,
                                                root );
            }

            DO_ASSERTIONS( root == my_rank
                           || remote == my_rank,
                           number_of_elements )

            free( recived_data );
            free( expected_data );
        }
    }
} /* test_ipc_send_recive_one_to_all */


void
test_ipc_send_recive_all_to_all( CuTest* tc )
{
    int my_rank            = SCOREP_Ipc_GetRank();
    int number_of_ranks    = SCOREP_Ipc_GetSize();
    int return_value       = -1;
    int number_of_elements = 10;

    int rank_is_even            = !( my_rank % 2 );
    int the_last_remaining_rank = ( my_rank == number_of_ranks - 1 );

    int* recived_data  = NULL;
    int* expected_data = NULL;

    if ( rank_is_even
         && !the_last_remaining_rank )
    {
        /*
         * P0: { 1, 2, 3, 4, 5, 6, 7, 8, 9,10}
         * P1: { 2, 3, 4, 5, 6, 7, 8, 9,10,11}
         * P2: { 3, 4, 5, 6, 7, 8, 9,10,11,12}
         */
        int* data_to_send = calloc( number_of_elements, sizeof( int ) );
        for ( int i = 0; i < number_of_elements; ++i )
        {
            data_to_send[ i ] = i + my_rank + 1;
        }

        return_value = SCOREP_Ipc_Send( data_to_send,
                                        number_of_elements,
                                        SCOREP_IPC_INT,
                                        my_rank + 1 );

        free( data_to_send );
    }
    else if ( !rank_is_even )
    {
        recived_data  = calloc( number_of_elements, sizeof( int ) );
        expected_data = calloc( number_of_elements, sizeof( int ) );
        for ( int i = 0; i < number_of_elements; ++i )
        {
            expected_data[ i ] = i + my_rank;
        }

        return_value = SCOREP_Ipc_Recv( recived_data,
                                        number_of_elements,
                                        SCOREP_IPC_INT,
                                        my_rank - 1 );
    }

    DO_ASSERTIONS( !the_last_remaining_rank,
                   number_of_elements )

    free( recived_data );
    free( expected_data );
} /* test_ipc_send_recive_all_to_all */
