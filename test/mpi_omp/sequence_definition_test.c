/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file       sequence_definition_test.c
 */

#include <config.h>
#include <mpi.h>
#include <omp.h>
#include <SCOREP_User.h>
#include <inttypes.h>

int
main( int argc, char** argv )
{
    int rank;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    SCOREP_USER_PARAMETER_INT64( "MPI rank", rank );

  #pragma omp parallel
    {
        uint64_t thread_id = ( ( uint64_t )omp_get_thread_num() ) << 32;
        thread_id += rank;
        SCOREP_USER_PARAMETER_INT64( "Thread id", thread_id );
    }

    MPI_Finalize();
    return 0;
}
