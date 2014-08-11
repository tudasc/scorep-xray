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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "test_ipc.h"
#include "tests/ipc_tests.h"

#include <scorep_ipc.h>

int
test_ipc( int           my_rank,
          TestAllreduce testAllreduce )
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNewParallel( "Ipc Interface Test-Suite",
                                           my_rank,
                                           testAllreduce );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_get_size,
                         "Number of Ranks > 0" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_get_rank,
                         "Number of current Rank > -1" );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_send_recive_one_to_one,
                         "Send and Recive: One to One" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_send_recive_all_to_one,
                         "Send and Recive: All to One" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_send_recive_one_to_all,
                         "Send and Recive: One to All" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_send_recive_all_to_all,
                         "Send and Recive: All to All" );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_broadcast,
                         "Broadcast" );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_scatter,
                         "Scatter" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_scatterv,
                         "Scatterv" );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_gather,
                         "Gather" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_gatherv,
                         "Gatherv" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_gatherv0,
                         "Gatherv(sendcount=0)" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_allgather,
                         "Allgather" );

    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_reduce,
                         "Reduce" );
    SUITE_ADD_TEST_NAME( suite,
                         test_ipc_allreduce,
                         "Allreduce" );

    SCOREP_Ipc_Init();
    CuSuiteRun( suite );
    SCOREP_Ipc_Finalize();

    CuSuiteSummary( suite, output );

    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
} /* ipc_test */
