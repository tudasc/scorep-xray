/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


#include <config.h>

#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>


#include "collectives_byte_calc_test.h"
#include "collectives_byte_calc_test_units.h"

#include <CuTest.h>

int
main( int argc, char** argv )
{
    MPI_Init( &argc, &argv );

    CuSuite* suite = init_test( 4 );

    if ( suite )
    {
        SUITE_ADD_TEST_NAME( suite, test_barrier, "barrier" );
        SUITE_ADD_TEST_NAME( suite, test_gather, "gather" );
        SUITE_ADD_TEST_NAME( suite, test_gather_inplace, "gather inplace" );
        SUITE_ADD_TEST_NAME( suite, test_reduce, "reduce" );
        SUITE_ADD_TEST_NAME( suite, test_gatherv, "gatherv" );
        SUITE_ADD_TEST_NAME( suite, test_gatherv_inplace, "gatherv inplace" );
        SUITE_ADD_TEST_NAME( suite, test_bcast, "bcast" );
        SUITE_ADD_TEST_NAME( suite, test_scatter, "scatter" );
        SUITE_ADD_TEST_NAME( suite, test_scatterv, "scatterv" );
        SUITE_ADD_TEST_NAME( suite, test_alltoall, "alltoall" );
        SUITE_ADD_TEST_NAME( suite, test_alltoall_inplace, "alltoall inplace" );
        SUITE_ADD_TEST_NAME( suite, test_alltoallv, "alltoallv" );
        SUITE_ADD_TEST_NAME( suite, test_alltoallv_inplace, "alltoallv inplace" );
        SUITE_ADD_TEST_NAME( suite, test_alltoallw, "alltoallw" );
        SUITE_ADD_TEST_NAME( suite, test_allgather, "allgather" );
        SUITE_ADD_TEST_NAME( suite, test_allgatherv, "allgatherv" );
        SUITE_ADD_TEST_NAME( suite, test_allgatherv_inplace, "allgatherv inplace" );
        SUITE_ADD_TEST_NAME( suite, test_allreduce, "allreduce" );
        SUITE_ADD_TEST_NAME( suite, test_reduce_scatter_block, "reduce_scatter_block" );
        SUITE_ADD_TEST_NAME( suite, test_reduce_scatter, "reduce_scatter" );
        SUITE_ADD_TEST_NAME( suite, test_scan, "scan" );
        SUITE_ADD_TEST_NAME( suite, test_exscan, "exscan" );

        CuSuiteRun( suite );
    }

    int failCount = finalize_test( suite );
    MPI_Finalize();

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
}
