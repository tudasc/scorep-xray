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

#ifndef COLLECTIVES_BYTE_CALC_TEST_UNITS_H
#define COLLECTIVES_BYTE_CALC_TEST_UNITS_H

#include <CuTest.h>

void
test_barrier( CuTest* ct );

void
test_gather( CuTest* ct );

void
test_gather_inplace( CuTest* ct );

void
test_reduce( CuTest* ct );

void
test_gatherv( CuTest* ct );

void
test_gatherv_inplace( CuTest* ct );

void
test_bcast( CuTest* ct );

void
test_scatter( CuTest* ct );

void
test_scatterv( CuTest* ct );

void
test_alltoall( CuTest* ct );

void
test_alltoall_inplace( CuTest* ct );

void
test_alltoallv( CuTest* ct );

void
test_alltoallv_inplace( CuTest* ct );

void
test_alltoallw( CuTest* ct );

void
test_allgather( CuTest* ct );

void
test_allgatherv( CuTest* ct );

void
test_allgatherv_inplace( CuTest* ct );

void
test_allreduce( CuTest* ct );

void
test_reduce_scatter_block( CuTest* ct );

void
test_reduce_scatter( CuTest* ct );

void
test_scan( CuTest* ct );

void
test_exscan( CuTest* ct );


#endif /* COLLECTIVES_BYTE_CALC_TEST_UNITS_H */
