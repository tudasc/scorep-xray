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

#ifndef COLLECTIVES_BYTE_CALC_TEST_H
#define COLLECTIVES_BYTE_CALC_TEST_H

#include <mpi.h>

#include <stdint.h>
#include <stdbool.h>


#include <CuTest.h>

extern MPI_Datatype Int32x1;
extern MPI_Datatype Int32x2;
extern MPI_Datatype Int32x4;
extern MPI_Datatype Int32x5;
extern MPI_Datatype Int32x10;
extern MPI_Datatype BogusType;

extern const uint32_t bogus32;
extern const uint64_t bogus64;

typedef struct ByteCounts
{
    uint64_t send;
    uint64_t recv;
} ByteCounts;

extern MPI_Comm testComm;

CuSuite*
init_test( int size );

int
finalize_test( CuSuite* );

MPI_Comm
create_comm( int size );

void
reduce_results( int* result );

void
validate_comm_size( CuTest* ct,
                    int     expected );

void
validate_byte_counts( CuTest*    ct,
                      ByteCounts actual,
                      ByteCounts expected );


#endif /* COLLECTIVES_BYTE_CALC_TEST_H */
