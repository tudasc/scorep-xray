/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  @brief A program for testing the instrumentation of MPI-File-I/O operations.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define CALL_MPI( func, args ... )                                           \
    do                                                                      \
    {                                                                       \
        if ( func( args ) != MPI_SUCCESS )                                   \
        {                                                                   \
            printf( "Error while calling %s on line %d\n", #func, __LINE__ ); \
        }                                                                   \
    } while ( 0 )


#define FILENAME "sample_out.txt"

void
write_into_file( char*      buffer,
                 MPI_Offset offset,
                 int        nbytes )
{
    MPI_Status status;
    MPI_File   fh;

    CALL_MPI( MPI_File_open, MPI_COMM_WORLD,
              FILENAME,
              MPI_MODE_CREATE | MPI_MODE_WRONLY,
              MPI_INFO_NULL,
              &fh );

    CALL_MPI( MPI_File_seek, fh, offset, MPI_SEEK_SET );

    CALL_MPI( MPI_File_write, fh,
              buffer,
              nbytes,
              MPI_CHAR,
              &status );

    CALL_MPI( MPI_File_close, &fh );
}

void
read_from_file( char*      buffer,
                MPI_Offset offset,
                int        nbytes )
{
    MPI_Status status;
    MPI_File   fh;

    CALL_MPI( MPI_File_open, MPI_COMM_WORLD,
              FILENAME,
              MPI_MODE_RDONLY,
              MPI_INFO_NULL,
              &fh );

    CALL_MPI( MPI_File_read_at, fh,
              offset,
              buffer,
              nbytes,
              MPI_CHAR,
              &status );

    CALL_MPI( MPI_File_close, &fh );
}

int
main( int argc, char** argv )
{
    int rank, size;
    int nbytes = atoi( argv[ 1 ] );

    char* write_buffer = ( char* )malloc( sizeof( char ) * nbytes );
    char* read_buffer  = ( char* )malloc( sizeof( char ) * nbytes );
    ;

    CALL_MPI( MPI_Init, &argc, &argv );
    CALL_MPI( MPI_Comm_size, MPI_COMM_WORLD, &size );
    CALL_MPI( MPI_Comm_rank, MPI_COMM_WORLD, &rank );

    for ( int i = 0; i < nbytes; ++i )
    {
        write_buffer[ i ] = rank;
    }

    write_into_file( write_buffer,
                     nbytes * sizeof( char ) * rank,
                     nbytes );

    read_from_file( read_buffer,
                    nbytes * sizeof( char ) * rank,
                    nbytes );

    for ( int i = 0; i < nbytes; ++i )
    {
        if ( read_buffer[ i ] != write_buffer[ i ] )
        {
            printf( "I/O transfer error: read[%d] != write[%d]\n", i, i );
        }
    }

    CALL_MPI( MPI_Barrier, MPI_COMM_WORLD );

    if ( rank == 0 )
    {
        CALL_MPI( MPI_File_delete, FILENAME, MPI_INFO_NULL );
    }

    CALL_MPI( MPI_Finalize );

    free( write_buffer );
    free( read_buffer );

    return EXIT_SUCCESS;
}
