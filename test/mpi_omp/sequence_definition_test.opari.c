#include <config.h>
#include "sequence_definition_test.c.opari.inc"
#line 1 "/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c"
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

#include <mpi.h>
#include <SCOREP_User.h>
#include <inttypes.h>

int
main( int argc, char** argv )
{
    int rank;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    SCOREP_USER_PARAMETER_INT64( "MPI rank", rank );

    {
        int               pomp2_num_threads = omp_get_max_threads();
        int               pomp2_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &opari2_region_1, pomp2_if, pomp2_num_threads, &pomp2_old_task, opari2_ctc_1 );
#line 31 "/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c"
  #pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
        { POMP2_Parallel_begin( &opari2_region_1 );
#line 32 "/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c"
          {
              uint64_t thread_id = ( ( uint64_t )omp_get_thread_num() ) << 32;
              thread_id += rank;
              SCOREP_USER_PARAMETER_INT64( "Thread id", thread_id );
          }
          { POMP2_Task_handle pomp2_old_task;
            POMP2_Implicit_barrier_enter( &opari2_region_1, &pomp2_old_task );
#pragma omp barrier
            POMP2_Implicit_barrier_exit( &opari2_region_1, pomp2_old_task );
          }
          POMP2_Parallel_end( &opari2_region_1 );
        }
        POMP2_Parallel_join( &opari2_region_1, pomp2_old_task );
    }
#line 37 "/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c"

    MPI_Finalize();
    return 0;
}
