#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#include "mpi_omp_hello_world.c.opari.inc"
#line 1 "mpi_omp_hello_world.c"
/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       mpi_omp_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdio.h>
#include <mpi.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


int
main( int    argc,
      char** argv )
{
    int rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    {
        int pomp_num_threads = omp_get_max_threads();
        POMP_Parallel_fork( pomp_region_1, pomp_num_threads );
#line 45 "mpi_omp_test.c"
    #pragma omp parallel POMP_DLIST_00001 num_threads(pomp_num_threads) copyin(pomp_tpd)
        { POMP_Parallel_begin( pomp_region_1 );
#line 46 "mpi_omp_test.c"
          {
              printf( "Hello world from process %d, thread %d of %d, %d\n",
                      rank, omp_get_thread_num(), size, omp_get_num_threads() );
          }
          POMP_Barrier_enter( pomp_region_1 );
#pragma omp barrier
          POMP_Barrier_exit( pomp_region_1 );
          POMP_Parallel_end( pomp_region_1 );
        }
        POMP_Parallel_join( pomp_region_1 );
    }
#line 50 "mpi_omp_test.c"

    MPI_Finalize();

    return 0;
}
