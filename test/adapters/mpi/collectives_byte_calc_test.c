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
#include "collectives_byte_calc_test.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

MPI_Datatype Int32x1;
MPI_Datatype Int32x2;
MPI_Datatype Int32x4;
MPI_Datatype Int32x5;
MPI_Datatype Int32x10;
MPI_Datatype BogusType;

const uint32_t bogus32 = 0xdeadbeef;
const uint64_t bogus64 = 0x1234567890abcedf;

MPI_Comm testComm;


CuSuite*
init_test( int size )
{
    testComm = create_comm( size );

    if ( testComm == MPI_COMM_NULL )
    {
        return NULL;
    }

    MPI_Type_contiguous( 1,  MPI_INT32_T, &Int32x1 );
    MPI_Type_commit( &Int32x1 );
    MPI_Type_contiguous( 2,  MPI_INT32_T, &Int32x2 );
    MPI_Type_commit( &Int32x2 );
    MPI_Type_contiguous( 4,  MPI_INT32_T, &Int32x4 );
    MPI_Type_commit( &Int32x4 );
    MPI_Type_contiguous( 5,  MPI_INT32_T, &Int32x5 );
    MPI_Type_commit( &Int32x5 );
    MPI_Type_contiguous( 10, MPI_INT32_T, &Int32x10 );
    MPI_Type_commit( &Int32x10 );

    MPI_Type_contiguous( 9876, MPI_CHAR, &BogusType );
    MPI_Type_commit( &BogusType );

    CuUseColors();
    int rank;
    MPI_Comm_rank( testComm, &rank );
    CuSuite* suite = CuSuiteNewParallel( "Collective byte calculation tests on 4 ranks",
                                         rank,
                                         reduce_results );
    return suite;
}

int
finalize_test( CuSuite* suite )
{
    if ( !suite )
    {
        return 0;
    }

    CuString* output = CuStringNew();
    CuSuiteSummary( suite, output );

    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    MPI_Type_free( &Int32x1 );
    MPI_Type_free( &Int32x2 );
    MPI_Type_free( &Int32x4 );
    MPI_Type_free( &Int32x5 );
    MPI_Type_free( &Int32x10 );

    MPI_Type_free( &BogusType );

    MPI_Comm_free( &testComm );

    return failCount;
}

MPI_Comm
create_comm( int size )
{
    int wsize, wrank;
    MPI_Comm_size( MPI_COMM_WORLD, &wsize );
    if ( size > wsize )
    {
        printf( "Requested a communicator with %d ranks, but world size is only %d\n", size, wsize );
        MPI_Abort( MPI_COMM_WORLD, MPI_ERR_COMM );
    }
    MPI_Comm_rank( MPI_COMM_WORLD, &wrank );

    int      color = ( wrank < size ) ? 1 : MPI_UNDEFINED;
    MPI_Comm comm;
    MPI_Comm_split( MPI_COMM_WORLD, color, wrank, &comm );
    return comm;
}

void
reduce_results( int* result )
{
    PMPI_Allreduce( MPI_IN_PLACE, result, 1, MPI_INT, MPI_MIN, testComm );
}

void
validate_comm_size( CuTest* ct, int expected )
{
    int commSize;
    MPI_Comm_size( testComm, &commSize );
    CuAssertIntEqualsMsg( ct, "Comm size", expected, commSize );
}

void
validate_byte_counts( CuTest* ct, ByteCounts actual, ByteCounts expected )
{
    ByteCounts expected_total = { 0, 0 };

    MPI_Allreduce( &expected.send, &expected_total.send, 1, MPI_UINT64_T, MPI_SUM, testComm );
    MPI_Allreduce( &expected.recv, &expected_total.recv, 1, MPI_UINT64_T, MPI_SUM, testComm );

    char error_message[ HUGE_STRING_LEN ];
    snprintf( error_message, HUGE_STRING_LEN,
              "Expected total sent <%" PRIu64 "> differs from expected total received <%" PRIu64 "> bytes",
              expected_total.send, expected_total.recv );
    CuAssertMsg( ct, error_message, expected_total.send == expected_total.recv );

    CuAssertIntEqualsMsg( ct, "Sent bytes", expected.send, actual.send );
    CuAssertIntEqualsMsg( ct, "Received bytes", expected.recv, actual.recv );
}
