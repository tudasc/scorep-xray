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
test_ipc_scatter( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();

    int number_of_elements = 2;
    int expected_data[]    =
    {
        my_rank,
        my_rank + 1
    };

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        int recived_data[] =
        {
            -1,
            -1
        };
        int* data_to_send = NULL;

        if ( root == my_rank )
        {
            /*
             * ranks = 3: {0,1,1,2,2,3}
             */
            data_to_send = calloc( number_of_ranks * number_of_elements, sizeof( int ) );
            int index = 0;
            for ( int i = 0; i < number_of_ranks; ++i )
            {
                data_to_send[ index++ ] = i;
                data_to_send[ index++ ] = i + 1;
            }
        }

        int return_value = SCOREP_Ipc_Scatter( data_to_send,
                                               recived_data,
                                               number_of_elements,
                                               SCOREP_IPC_INT,
                                               root );

        DO_ASSERTIONS( 1,
                       number_of_elements )

        free( data_to_send );
    }
} /* test_ipc_scatter */


void
test_ipc_scatterv( CuTest* tc )
{
    int my_rank                 = SCOREP_Ipc_GetRank();
    int number_of_ranks         = SCOREP_Ipc_GetSize();
    int number_of_elements_sent = CALCULATE_TRIANGULAR_NUMBER( number_of_ranks );

    int* counts = calloc( number_of_ranks, sizeof( int ) );
    for ( int i = 0; i < number_of_ranks; ++i )
    {
        counts[ i ] = i + 1;
    }

    int* recived_data  = calloc( counts[ my_rank ], sizeof( int ) );
    int* expected_data = calloc( counts[ my_rank ], sizeof( int ) );
    for ( int i = 0; i < counts[ my_rank ]; ++i )
    {
        expected_data[ i ] = my_rank;
    }

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        int* data_to_send = NULL;

        if ( root == my_rank )
        {
            /*
             * ranks = 3: {0,1,1,2,2,2,3,3,3,3}
             */
            data_to_send = calloc( number_of_elements_sent, sizeof( int ) );

            int index = 0;
            for ( int i = 0; i < number_of_ranks; ++i )
            {
                for ( int j = 0; j <= i; ++j )
                {
                    data_to_send[ index++ ] = i;
                }
            }
        }

        int return_value = SCOREP_Ipc_Scatterv( data_to_send,
                                                counts,
                                                recived_data,
                                                counts[ my_rank ],
                                                SCOREP_IPC_INT,
                                                root );

        CuAssertIntEqualsMsg( tc, "return value", 0, return_value );

        int i = 0;
        for (; i < counts[ my_rank ]; ++i )
        {
            CuAssertIntEquals( tc, expected_data[ i ], recived_data[ i ] );
        }

        for (; i < counts[ number_of_ranks - 1 ]; ++i )
        {
            CuAssertIntEqualsMsg( tc, "expected_data[ i ], recived_data", 0, 0 );
        }

        free( data_to_send );
    }

    free( counts );
    free( recived_data );
    free( expected_data );
} /* test_ipc_scatterv */
