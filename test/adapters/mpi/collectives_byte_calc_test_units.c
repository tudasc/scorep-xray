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
#include <scorep_mpi_coll.h>

void
test_barrier( CuTest* ct )
{
    validate_comm_size( ct, 4 );
    ByteCounts bytes    = { bogus64, bogus64 };
    ByteCounts expected = { 0, 0 };

    scorep_mpi_coll_bytes_barrier( testComm, &bytes.send, &bytes.recv );
    validate_byte_counts( ct, bytes, expected );
}

void
test_gather( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 0;

    int          sendcount = bogus32;
    int          recvcount = bogus32;
    MPI_Datatype sendtype  = BogusType;
    MPI_Datatype recvtype  = BogusType;
    switch ( rank )
    {
        /* root */
        case 0:
            sendcount = 100;
            sendtype  = Int32x1;
            recvcount = 50;
            recvtype  = Int32x2;
            break;
        /* other:
         * wrong values for recv are ignored
         */
        case 1:
            sendcount = 50;
            sendtype  = Int32x2;
            break;
        case 2:
            sendcount = 25;
            sendtype  = Int32x4;
            break;
        case 3:
            sendcount = 20;
            sendtype  = Int32x5;
            break;
    }
    scorep_mpi_coll_bytes_gather( sendcount, sendtype,
                                  recvcount, recvtype,
                                  root, inplace, testComm,
                                  &bytes.send, &bytes.recv );

    ByteCounts expected = { 0, 0 };
    if ( rank == root )
    {
        expected = ( ByteCounts ){.send = ( 4 * 100 ), .recv = 4 * ( 4 * 100 ) };
    }
    else
    {
        expected = ( ByteCounts ){.send = ( 4 * 100 ), .recv = 0 };
    }
    validate_byte_counts( ct, bytes, expected );
}

void
test_gather_inplace( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 1;

    int          sendcount = bogus32;
    int          recvcount = bogus32;
    MPI_Datatype sendtype  = BogusType;
    MPI_Datatype recvtype  = BogusType;
    switch ( rank )
    {
        /* root:
         * wrong values for send are ignored*/
        case 0:
            recvcount = 50;
            recvtype  = Int32x2;
            break;
        /* other:
         * wrong values for recv are ignored
         */
        case 1:
            sendcount = 50;
            sendtype  = Int32x2;
            break;
        case 2:
            sendcount = 25;
            sendtype  = Int32x4;
            break;
        case 3:
            sendcount = 20;
            sendtype  = Int32x5;
            break;
    }
    scorep_mpi_coll_bytes_gather( sendcount, sendtype,
                                  recvcount, recvtype,
                                  root, inplace, testComm,
                                  &bytes.send, &bytes.recv );

    ByteCounts expected = { 0, 0 };
    if ( rank == root )
    {
        expected = ( ByteCounts ){.send = 0, .recv = 3 * 100 * 4 };
    }
    else
    {
        expected = ( ByteCounts ){.send = 100 * 4, .recv = 0 };
    }
    validate_byte_counts( ct, bytes, expected );
}

void
test_reduce( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 0;

    /* count and datatype has to be the same for all ranks */
    int          count    = 50;
    MPI_Datatype datatype = Int32x2;

    scorep_mpi_coll_bytes_reduce( count, datatype,
                                  root, inplace, testComm,
                                  &bytes.send, &bytes.recv );

    ByteCounts expected = { 0, 0 };
    if ( rank == root )
    {
        expected = ( ByteCounts ){.send = 50 * ( 4 * 2 ), .recv = 4 * 50 * ( 4 * 2 ) };
    }
    else
    {
        expected = ( ByteCounts ){.send = 50 * ( 4 * 2 ), .recv = 0 };
    }
    validate_byte_counts( ct, bytes, expected );
}

void
test_gatherv( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 0;

    int          sendcount       = bogus32;
    int          recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    MPI_Datatype sendtype        = BogusType;
    MPI_Datatype recvtype        = BogusType;

    ByteCounts expected = { 0, 0 };
    switch ( rank )
    {
        /* root */
        case 0:
            sendcount       = 10;
            sendtype        = Int32x1;
            recvcounts[ 0 ] = 1;
            recvcounts[ 1 ] = 2;
            recvcounts[ 2 ] = 3;
            recvcounts[ 3 ] = 4;
            recvtype        = Int32x10;
            expected        = ( ByteCounts ){.send = ( 4 * 10 ), .recv = ( 1 + 2 + 3 + 4 ) * ( 4 * 10 ) };
            break;
        /* other:
         * wrong values for recv are ignored
         */
        case 1:
            sendcount = 20;
            sendtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 20 * ( 4 * 1 ), .recv = 0 };
            break;
        case 2:
            sendcount = 30;
            sendtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 30 * ( 4 * 1 ), .recv = 0 };
            break;
        case 3:
            sendcount = 20;
            sendtype  = Int32x2;
            expected  = ( ByteCounts ){.send = 20 * ( 4 * 2 ), .recv = 0 };
            break;
    }
    scorep_mpi_coll_bytes_gatherv( sendcount, sendtype,
                                   recvcounts, recvtype,
                                   root, inplace, testComm,
                                   &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}


void
test_gatherv_inplace( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 1;

    int          sendcount       = bogus32;
    int          recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    MPI_Datatype sendtype        = BogusType;
    MPI_Datatype recvtype        = BogusType;

    ByteCounts expected = { 0, 0 };
    switch ( rank )
    {
        /* root:
         * wrong values for send are ignored
         */
        case 0:
            recvcounts[ 0 ] = 1;
            recvcounts[ 1 ] = 2;
            recvcounts[ 2 ] = 3;
            recvcounts[ 3 ] = 4;
            recvtype        = Int32x10;
            expected        = ( ByteCounts ){.send = 0, .recv = ( 2 + 3 + 4 ) * ( 4 * 10 ) };
            break;
        /* other:
         * wrong values for recv are ignored
         */
        case 1:
            sendcount = 20;
            sendtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 20 * ( 4 * 1 ), .recv = 0 };
            break;
        case 2:
            sendcount = 30;
            sendtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 30 * ( 4 * 1 ), .recv = 0 };
            break;
        case 3:
            sendcount = 20;
            sendtype  = Int32x2;
            expected  = ( ByteCounts ){.send = 20 * ( 4 * 2 ), .recv = 0 };
            break;
    }
    scorep_mpi_coll_bytes_gatherv( sendcount, sendtype,
                                   recvcounts, recvtype,
                                   root, inplace, testComm,
                                   &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}

void
test_bcast( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root = 0;

    int          count    = bogus32;
    MPI_Datatype datatype = BogusType;
    switch ( rank )
    {
        /* root */
        case 0:
            count    = 100;
            datatype = Int32x1;
            break;
        case 1:
            count    = 50;
            datatype = Int32x2;
            break;
        case 2:
            count    = 25;
            datatype = Int32x4;
            break;
        case 3:
            count    = 10;
            datatype = Int32x10;
            break;
    }
    scorep_mpi_coll_bytes_bcast( count, datatype,
                                 root, testComm,
                                 &bytes.send, &bytes.recv );

    ByteCounts expected = { 0, 0 };
    if ( rank == root )
    {
        expected = ( ByteCounts ){.send = 4 * ( 4 * 100 ), .recv = ( 4 * 100 ) };
    }
    else
    {
        expected = ( ByteCounts ){.send = 0, .recv = ( 4 * 100 ) };
    }
    validate_byte_counts( ct, bytes, expected );
}

void
test_scatter( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 0;

    int          sendcount = bogus32;
    int          recvcount = bogus32;
    MPI_Datatype sendtype  = BogusType;
    MPI_Datatype recvtype  = BogusType;
    switch ( rank )
    {
        /* root */
        case 0:
            sendcount = 100;
            sendtype  = Int32x1;
            recvcount = 10;
            recvtype  = Int32x10;
            break;
        /* other:
         * wrong values for send are ignored
         */
        case 1:
            recvcount = 25;
            recvtype  = Int32x4;
            break;
        case 2:
            recvcount = 20;
            recvtype  = Int32x5;
            break;
        case 3:
            recvcount = 100;
            recvtype  = Int32x1;
            break;
    }
    scorep_mpi_coll_bytes_scatter( sendcount, sendtype,
                                   recvcount, recvtype,
                                   root, inplace, testComm,
                                   &bytes.send, &bytes.recv );

    ByteCounts expected = { 0, 0 };
    if ( rank == root )
    {
        expected = ( ByteCounts ){.send = 4 * ( 4 * 100 ), .recv = ( 4 * 100 ) };
    }
    else
    {
        expected = ( ByteCounts ){.send = 0, .recv = ( 4 * 100 ) };
    }
    validate_byte_counts( ct, bytes, expected );
}

void
test_scatterv( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int root    = 0;
    int inplace = 0;

    int sendcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    int recvcount       = bogus32;

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    ByteCounts expected = { 0, 0 };
    switch ( rank )
    {
        /* root */
        case 0:
            sendcounts[ 0 ] = 4;
            sendcounts[ 1 ] = 3;
            sendcounts[ 2 ] = 2;
            sendcounts[ 3 ] = 1;
            sendtype        = Int32x10;
            recvcount       = 40;
            recvtype        = Int32x1;
            expected        = ( ByteCounts ){.send = ( 4 + 3 + 2 + 1 ) * ( 4 * 10 ), .recv = 40 * ( 4 * 1 ) };
            break;
        case 1:
            recvcount = 30;
            recvtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 0, .recv = 30 * ( 4 * 1 ) };
            break;
        case 2:
            recvcount = 20;
            recvtype  = Int32x1;
            expected  = ( ByteCounts ){.send = 0, .recv = 20 * ( 4 * 1 ) };
            break;
        case 3:
            recvcount = 5;
            recvtype  = Int32x2;
            expected  = ( ByteCounts ){.send = 0, .recv = 5 * ( 4 * 2 ) };
            break;
    }
    scorep_mpi_coll_bytes_scatterv( sendcounts, sendtype,
                                    recvcount, recvtype,
                                    root, inplace, testComm,
                                    &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}


void
test_alltoall( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    int sendcount = bogus32;
    int recvcount = bogus32;

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    switch ( rank )
    {
        case 0: /*fallthrough*/
        case 2:
            sendcount = 100;
            sendtype  = Int32x1;
            recvcount = 10;
            recvtype  = Int32x10;
            break;
        case 1: /*fallthrough*/
        case 3:
            sendcount = 50;
            sendtype  = Int32x2;
            recvcount = 25;
            recvtype  = Int32x4;
            break;
    }
    scorep_mpi_coll_bytes_alltoall( sendcount, sendtype,
                                    recvcount, recvtype,
                                    inplace, testComm,
                                    &bytes.send, &bytes.recv );

    ByteCounts expected = { .send = 4 * ( 4 * 100 ), .recv = 4 * ( 4 * 100 ) };

    validate_byte_counts( ct, bytes, expected );
}

void
test_alltoall_inplace( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 1;

    int sendcount = bogus32;
    int recvcount = bogus32;

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    switch ( rank )
    {
        case 0: /*fallthrough*/
        case 2:
            recvcount = 10;
            recvtype  = Int32x10;
            break;
        case 1: /*fallthrough*/
        case 3:
            recvcount = 25;
            recvtype  = Int32x4;
            break;
    }
    /* wrong values for sendcount, sendtype are ignored */
    scorep_mpi_coll_bytes_alltoall( sendcount, sendtype,
                                    recvcount, recvtype,
                                    inplace, testComm,
                                    &bytes.send, &bytes.recv );

    ByteCounts expected = { .send = 3 * ( 4 * 100 ), .recv = 3 * ( 4 * 100 ) };

    validate_byte_counts( ct, bytes, expected );
}

void
test_alltoallv( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    int sendcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    int recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    ByteCounts expected = { 0, 0 };
    /* *INDENT-OFF* */
    switch(rank)
    {
        case 0:
            sendcounts[0] =   0; sendcounts[1] =  10; sendcounts[2] =  20; sendcounts[3] =  30;
            sendtype = Int32x1;
            recvcounts[0] =   0; recvcounts[1] =  20; recvcounts[2] =  80; recvcounts[3] = 150;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(0+10+20+30)*(4*1), .recv=(0+20+80+150)*(4*1)};
            break;
        case 1:
            sendcounts[0] =  10; sendcounts[1] =  20; sendcounts[2] =  30; sendcounts[3] =  0;
            sendtype = Int32x2;
            recvcounts[0] =  10; recvcounts[1] =  40; recvcounts[2] = 120; recvcounts[3] =  0;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(10+20+30+0)*(4*2), .recv=(10+40+120+0)*(4*1)};
            break;
        case 2:
            sendcounts[0] =  20; sendcounts[1] =  30; sendcounts[2] =   0; sendcounts[3] =  10;
            sendtype = Int32x4;
            recvcounts[0] =  20; recvcounts[1] =  60; recvcounts[2] =   0; recvcounts[3] =  50;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(20+30+0+10)*(4*4), .recv=(20+60+0+50)*(4*1)};
            break;
        case 3:
            sendcounts[0] =  30; sendcounts[1] =   0; sendcounts[2] =  10; sendcounts[3] =  20;
            sendtype = Int32x5;
            recvcounts[0] =  30; recvcounts[1] =   0; recvcounts[2] =  40; recvcounts[3] = 100;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(30+0+10+20)*(4*5), .recv=(30+0+40+100)*(4*1)};
            break;
    }
    /* *INDENT-ON* */
    scorep_mpi_coll_bytes_alltoallv( sendcounts, sendtype,
                                     recvcounts, recvtype,
                                     inplace, testComm,
                                     &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}

void
test_alltoallv_inplace( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 1;

    int sendcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    int recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    ByteCounts expected = { 0, 0 };
    /* *INDENT-OFF* */
    switch(rank)
    {
        case 0:
            recvcounts[0] =  10; recvcounts[1] =  20; recvcounts[2] =  30; recvcounts[3] = 40;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(0+20+30+40)*(4*1), .recv=(0+20+30+40)*(4*1)};
            break;
        case 1:
            recvcounts[0] =  20; recvcounts[1] =  20; recvcounts[2] =  30; recvcounts[3] = 40;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(20+0+30+40)*(4*1), .recv=(20+0+30+40)*(4*1)};
            break;
        case 2:
            recvcounts[0] =  30; recvcounts[1] =  30; recvcounts[2] =  30; recvcounts[3] = 40;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(30+30+0+40)*(4*1), .recv=(30+30+0+40)*(4*1)};
            break;
        case 3:
            recvcounts[0] =  40; recvcounts[1] =  40; recvcounts[2] =  40; recvcounts[3] = 40;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=(40+40+40+0)*(4*1), .recv=(40+40+40+0)*(4*1)};
            break;
    }
    /* *INDENT-ON* */
    /* wrong values for sendcounts, sendtype are ignored */
    scorep_mpi_coll_bytes_alltoallv( sendcounts, sendtype,
                                     recvcounts, recvtype,
                                     inplace, testComm,
                                     &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}

void
test_alltoallw( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    int sendcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };
    int recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };

    MPI_Datatype sendtypes[ 4 ] = { BogusType, BogusType, BogusType, BogusType };
    MPI_Datatype recvtypes[ 4 ] = { BogusType, BogusType, BogusType, BogusType };

    ByteCounts expected = { 0, 0 };
    /* *INDENT-OFF* */
    switch(rank)
    {
        case 0:
            sendcounts[0] =      10; sendcounts[1] =      10; sendcounts[2] =      10; sendcounts[3] =      10;
             sendtypes[0] = Int32x1;  sendtypes[1] = Int32x2;  sendtypes[2] = Int32x4;  sendtypes[3] = Int32x5;

            recvcounts[0] =      10; recvcounts[1] =      20; recvcounts[2] =      30; recvcounts[3] =      40;
             recvtypes[0] = Int32x1;  recvtypes[1] = Int32x5;  recvtypes[2] = Int32x1;  recvtypes[3] = Int32x2;
            expected = (ByteCounts){.send=10*(4*1)+10*(4*2)+10*(4*4)+10*(4*5), .recv=10*(4*1)+20*(4*5)+30*(4*1)+40*(4*2)};
            break;
        case 1:
            sendcounts[0] =      20; sendcounts[1] =      20; sendcounts[2] =      20; sendcounts[3] =      20;
             sendtypes[0] = Int32x5;  sendtypes[1] = Int32x4;  sendtypes[2] = Int32x2;  sendtypes[3] = Int32x1;

            recvcounts[0] =      10; recvcounts[1] =      20; recvcounts[2] =     30; recvcounts[3] =       40;
             recvtypes[0] = Int32x2;  recvtypes[1] = Int32x4;  recvtypes[2] = Int32x5;  recvtypes[3] = Int32x4;
            expected = (ByteCounts){.send=20*(4*5)+20*(4*4)+20*(4*2)+20*(4*1), .recv=10*(4*2)+20*(4*4)+30*(4*5)+40*(4*4)};
            break;
        case 2:
            sendcounts[0] =      30; sendcounts[1] =      30; sendcounts[2] =      30; sendcounts[3] =      30;
             sendtypes[0] = Int32x1;  sendtypes[1] = Int32x5;  sendtypes[2] = Int32x2;  sendtypes[3] = Int32x4;

            recvcounts[0] =      10; recvcounts[1] =      20; recvcounts[2] =      30; recvcounts[3] =      40;
             recvtypes[0] = Int32x4;  recvtypes[1] = Int32x2;  recvtypes[2] = Int32x2;  recvtypes[3] = Int32x1;
            expected = (ByteCounts){.send=30*(4*1)+30*(4*5)+30*(4*2)+30*(4*4), .recv=10*(4*4)+20*(4*2)+30*(4*2)+40*(4*1)};
            break;
        case 3:
            sendcounts[0] =      40; sendcounts[1] =      40; sendcounts[2] =      40; sendcounts[3] =      40;
             sendtypes[0] = Int32x2;  sendtypes[1] = Int32x4;  sendtypes[2] = Int32x1;  sendtypes[3] = Int32x5;

            recvcounts[0] =      10; recvcounts[1] =      20; recvcounts[2] =      30; recvcounts[3] =     40;
             recvtypes[0] = Int32x5;  recvtypes[1] = Int32x1;  recvtypes[2] = Int32x4;  recvtypes[3] = Int32x5;
            expected = (ByteCounts){.send=40*(4*2)+40*(4*4)+40*(4*1)+40*(4*5), .recv=10*(4*5)+20*(4*1)+30*(4*4)+40*(4*5)};
            break;
    }
    /* *INDENT-ON* */
    scorep_mpi_coll_bytes_alltoallw( sendcounts, sendtypes,
                                     recvcounts, recvtypes,
                                     inplace, testComm,
                                     &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}

void
test_allgather( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    int sendcount = bogus32;
    int recvcount = bogus32;

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    switch ( rank )
    {
        case 0:
            sendcount = 100;
            sendtype  = Int32x1;
            recvcount = 100;
            recvtype  = Int32x1;
            break;
        case 2:
            sendcount = 100;
            sendtype  = Int32x1;
            recvcount = 10;
            recvtype  = Int32x10;
            break;
        case 1:
            sendcount = 10;
            sendtype  = Int32x10;
            recvcount = 20;
            recvtype  = Int32x5;
            break;
        case 3:
            sendcount = 20;
            sendtype  = Int32x5;
            recvcount = 25;
            recvtype  = Int32x4;
            break;
    }
    scorep_mpi_coll_bytes_allgather( sendcount, sendtype,
                                     recvcount, recvtype,
                                     inplace, testComm,
                                     &bytes.send, &bytes.recv );

    ByteCounts expected = { .send = 4 * ( 4 * 100 ), .recv = 4 * ( 4 * 100 ) };

    validate_byte_counts( ct, bytes, expected );
}

void
test_allgatherv( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    int sendcount       = bogus32;
    int recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    ByteCounts expected = { 0, 0 };

    /* *INDENT-OFF* */
    switch(rank)
    {
        case 0:
            sendcount = 100;
            sendtype = Int32x1;
            recvcounts[0] =   20; recvcounts[1] =  40; recvcounts[2] =  80; recvcounts[3] = 100;
            recvtype = Int32x5;
            expected = (ByteCounts){.send=4*100*(4*1), .recv=(240)*(4*5)};
            break;
        case 1:
            sendcount = 100;
            sendtype = Int32x2;
            recvcounts[0] =   25; recvcounts[1] =  50; recvcounts[2] =  100; recvcounts[3] = 125;
            recvtype = Int32x4;
            expected = (ByteCounts){.send=4*100*(4*2), .recv=(300)*(4*4)};
            break;
        case 2:
            sendcount = 100;
            sendtype = Int32x4;
            recvcounts[0] =   50; recvcounts[1] =  100; recvcounts[2] =  200; recvcounts[3] = 250;
            recvtype = Int32x2;
            expected = (ByteCounts){.send=4*100*(4*4), .recv=(600)*(4*2)};
            break;
        case 3:
            sendcount = 100;
            sendtype = Int32x5;
            recvcounts[0] =   100; recvcounts[1] =  200; recvcounts[2] =  400; recvcounts[3] = 500;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=4*100*(4*5), .recv=1200*(4*1)};
            break;
    }
    /* *INDENT-ON* */
    scorep_mpi_coll_bytes_allgatherv( sendcount, sendtype,
                                      recvcounts, recvtype,
                                      inplace, testComm,
                                      &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}

void
test_allgatherv_inplace( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 1;

    int sendcount       = bogus32;
    int recvcounts[ 4 ] = { bogus32, bogus32, bogus32, bogus32 };

    MPI_Datatype sendtype = BogusType;
    MPI_Datatype recvtype = BogusType;

    ByteCounts expected = { 0, 0 };

    /* *INDENT-OFF* */
    switch(rank)
    {
        case 0:
            recvcounts[0] =   20; recvcounts[1] =  40; recvcounts[2] =  80; recvcounts[3] = 100;
            recvtype = Int32x5;
            expected = (ByteCounts){.send=3*20*(4*5), .recv=(40+80+100)*(4*5)};
            break;
        case 1:
            recvcounts[0] =   25; recvcounts[1] =  50; recvcounts[2] =  100; recvcounts[3] = 125;
            recvtype = Int32x4;
            expected = (ByteCounts){.send=3*50*(4*4), .recv=(25+100+125)*(4*4)};
            break;
        case 2:
            recvcounts[0] =   50; recvcounts[1] =  100; recvcounts[2] =  200; recvcounts[3] = 250;
            recvtype = Int32x2;
            expected = (ByteCounts){.send=3*200*(4*2), .recv=(50+100+250)*(4*2)};
            break;
        case 3:
            recvcounts[0] =   100; recvcounts[1] =  200; recvcounts[2] =  400; recvcounts[3] = 500;
            recvtype = Int32x1;
            expected = (ByteCounts){.send=3*500*(4*1), .recv=(100+200+400)*(4*1)};
            break;
    }
    /* *INDENT-ON* */
    /* wrong values for sendcount, sendtype are ignored */
    scorep_mpi_coll_bytes_allgatherv( sendcount, sendtype,
                                      recvcounts, recvtype,
                                      inplace, testComm,
                                      &bytes.send, &bytes.recv );

    validate_byte_counts( ct, bytes, expected );
}


void
test_allreduce( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    /* count and datatype has to be the same for all ranks */
    int          count    = 25;
    MPI_Datatype datatype = Int32x4;

    scorep_mpi_coll_bytes_allreduce( count, datatype,
                                     inplace, testComm,
                                     &bytes.send, &bytes.recv );

    ByteCounts expected = { .send = 4 * 25 * ( 4 * 4 ), .recv = 4 * 25 * ( 4 * 4 ) };

    validate_byte_counts( ct, bytes, expected );
}

void
test_reduce_scatter_block( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    /* recvcounts and datatype has to be the same for all ranks */
    int          recvcount = 50;
    MPI_Datatype datatype  = Int32x2;

    scorep_mpi_coll_bytes_reduce_scatter_block( recvcount, datatype,
                                                inplace, testComm,
                                                &bytes.send, &bytes.recv );

    ByteCounts expected = { .send = 4 * 50 * ( 4 * 2 ), .recv = 4 * 50 * ( 4 * 2 ) };

    validate_byte_counts( ct, bytes, expected );
}


void
test_reduce_scatter( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    /* recvcounts and datatype has to be the same for all ranks */
    int          recvcounts[ 4 ] = { 10, 20, 30, 40 };
    MPI_Datatype datatype        = Int32x1;

    ByteCounts expected = { 0, 0 };

    switch ( rank )
    {
        case 0:
            expected = ( ByteCounts ){.send = 100 * ( 4 * 1 ), .recv = 4 * 10 * ( 4 * 1 ) };
            break;
        case 1:
            expected = ( ByteCounts ){.send = 100 * ( 4 * 1 ), .recv = 4 * 20 * ( 4 * 1 ) };
            break;
        case 2:
            expected = ( ByteCounts ){.send = 100 * ( 4 * 1 ), .recv = 4 * 30 * ( 4 * 1 ) };
            break;
        case 3:
            expected = ( ByteCounts ){.send = 100 * ( 4 * 1 ), .recv = 4 * 40 * ( 4 * 1 ) };
            break;
    }
    scorep_mpi_coll_bytes_reduce_scatter( recvcounts, datatype,
                                          inplace, testComm,
                                          &bytes.send, &bytes.recv );



    validate_byte_counts( ct, bytes, expected );
}

void
test_scan( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    /* count and datatype has to be the same for all ranks */
    int          count    = 100;
    MPI_Datatype datatype = Int32x1;

    ByteCounts expected = { 0, 0 };

    switch ( rank )
    {
        case 0:
            expected = ( ByteCounts ){.send = 4 * 100 * ( 4 * 1 ), .recv = 1 * 100 * ( 4 * 1 ) };
            break;
        case 1:
            expected = ( ByteCounts ){.send = 3 * 100 * ( 4 * 1 ), .recv = 2 * 100 * ( 4 * 1 ) };
            break;
        case 2:
            expected = ( ByteCounts ){.send = 2 * 100 * ( 4 * 1 ), .recv = 3 * 100 * ( 4 * 1 ) };
            break;
        case 3:
            expected = ( ByteCounts ){.send = 1 * 100 * ( 4 * 1 ), .recv = 4 * 100 * ( 4 * 1 ) };
            break;
    }
    scorep_mpi_coll_bytes_scan( count, datatype,
                                inplace, testComm,
                                &bytes.send, &bytes.recv );



    validate_byte_counts( ct, bytes, expected );
}

void
test_exscan( CuTest* ct )
{
    validate_comm_size( ct, 4 );

    int rank;
    MPI_Comm_rank( testComm, &rank );

    ByteCounts bytes = { bogus64, bogus64 };

    int inplace = 0;

    /* count and datatype has to be the same for all ranks */
    int          count    = 100;
    MPI_Datatype datatype = Int32x1;

    ByteCounts expected = { 0, 0 };

    switch ( rank )
    {
        case 0:
            expected = ( ByteCounts ){.send = 3 * 100 * ( 4 * 1 ), .recv = 0 * 100 * ( 4 * 1 ) };
            break;
        case 1:
            expected = ( ByteCounts ){.send = 2 * 100 * ( 4 * 1 ), .recv = 1 * 100 * ( 4 * 1 ) };
            break;
        case 2:
            expected = ( ByteCounts ){.send = 1 * 100 * ( 4 * 1 ), .recv = 2 * 100 * ( 4 * 1 ) };
            break;
        case 3:
            expected = ( ByteCounts ){.send = 0 * 100 * ( 4 * 1 ), .recv = 3 * 100 * ( 4 * 1 ) };
            break;
    }
    scorep_mpi_coll_bytes_exscan( count, datatype,
                                  inplace, testComm,
                                  &bytes.send, &bytes.recv );



    validate_byte_counts( ct, bytes, expected );
}
