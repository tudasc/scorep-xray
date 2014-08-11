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
test_ipc_reduce( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();

    int                  number_of_operations = 5;
    SCOREP_Ipc_Operation operations[]         =
    {
        SCOREP_IPC_MIN,
        SCOREP_IPC_MAX,
        SCOREP_IPC_SUM,
        SCOREP_IPC_BAND,
        SCOREP_IPC_BOR
    };

    int expected_data[] =
    {
        1,               /* SCOREP_IPC_MIN */
        number_of_ranks, /* SCOREP_IPC_MAX */
        1,               /* SCOREP_IPC_SUM */
        1,               /* SCOREP_IPC_BAND */
        1                /* SCOREP_IPC_BOR */
    };
    for ( int i = 1; i < number_of_ranks; ++i )
    {
        expected_data[ 2 ] += i + 1;
        expected_data[ 3 ] &= i + 1;
        expected_data[ 4 ] |= i + 1;
    }

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        for ( int op = 0; op < number_of_operations; ++op )
        {
            int  data_to_send[ 1 ] = { my_rank + 1 };
            int* result            = NULL;
            int  result_data       = -1;

            if ( root == my_rank )
            {
                result = &result_data;
            }

            int return_value = SCOREP_Ipc_Reduce( data_to_send,
                                                  result,
                                                  1,
                                                  SCOREP_IPC_INT,
                                                  operations[ op ],
                                                  root );

            CuAssertIntEqualsMsg( tc, "return value", 0, return_value );
            if ( root == my_rank )
            {
                CuAssertIntEquals( tc, expected_data[ op ], result_data );
            }
            else
            {
                CuAssertIntEqualsMsg( tc, "expected_data[ op ], result_data", 0, 0 );
            }
        }
    }
} /* test_ipc_reduce */


void
test_ipc_allreduce( CuTest* tc )
{
    int my_rank         = SCOREP_Ipc_GetRank();
    int number_of_ranks = SCOREP_Ipc_GetSize();

    int                  number_of_operations = 5;
    SCOREP_Ipc_Operation operations[]         =
    {
        SCOREP_IPC_MIN,
        SCOREP_IPC_MAX,
        SCOREP_IPC_SUM,
        SCOREP_IPC_BAND,
        SCOREP_IPC_BOR
    };

    int expected_data[] =
    {
        1,               /* SCOREP_IPC_MIN */
        number_of_ranks, /* SCOREP_IPC_MAX */
        1,               /* SCOREP_IPC_SUM */
        1,               /* SCOREP_IPC_BAND */
        1                /* SCOREP_IPC_BOR */
    };
    for ( int i = 1; i < number_of_ranks; ++i )
    {
        expected_data[ 2 ] += i + 1;
        expected_data[ 3 ] &= i + 1;
        expected_data[ 4 ] |= i + 1;
    }

    for ( int op = 0; op < number_of_operations; ++op )
    {
        int data_to_send = my_rank + 1;
        int result       = -1;

        int return_value = SCOREP_Ipc_Allreduce( &data_to_send,
                                                 &result,
                                                 1,
                                                 SCOREP_IPC_INT,
                                                 operations[ op ] );

        CuAssertIntEqualsMsg( tc, "return value", 0, return_value );
        CuAssertIntEquals( tc, expected_data[ op ], result );
    }
} /* test_ipc_allreduce */
