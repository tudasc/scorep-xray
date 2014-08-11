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
test_ipc_get_size( CuTest* tc )
{
    int number_of_ranks = -1;

    number_of_ranks = SCOREP_Ipc_GetSize();
    CuAssert( tc, "should be", number_of_ranks > 0 );
} /* test_ipc_get_size */


void
test_ipc_get_rank( CuTest* tc )
{
    int my_rank = -1;

    my_rank = SCOREP_Ipc_GetRank();
    CuAssert( tc, "should be", my_rank > -1 );
} /* test_ipc_get_rank */


void
test_ipc_broadcast( CuTest* tc )
{
    int my_rank            = SCOREP_Ipc_GetRank();
    int number_of_ranks    = SCOREP_Ipc_GetSize();
    int number_of_elements = 10;

    int expected_data[ number_of_elements ];
    int recived_data[ number_of_elements ];
    for ( int i = 0; i < number_of_elements; ++i )
    {
        expected_data[ i ] = i + 1;
    }

    for ( int root = 0; root < number_of_ranks; ++root )
    {
        memset( recived_data, 0xff, number_of_elements * sizeof( 0 ) );
        if ( root == my_rank )
        {
            for ( int i = 0; i < number_of_elements; ++i )
            {
                recived_data[ i ] = i + 1;
            }
        }
        int return_value = SCOREP_Ipc_Bcast( recived_data,
                                             number_of_elements,
                                             SCOREP_IPC_INT,
                                             root );

        DO_ASSERTIONS( 1,
                       number_of_elements )
    }
} /* test_ipc_broadcast */
