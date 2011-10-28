#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#include "mpi_omp_hello_world.c.opari.inc"
#line 1 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c"
/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
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


#include <config.h>

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
        int               pomp_num_threads = omp_get_max_threads();
        int               pomp_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_1, pomp_if, pomp_num_threads, &pomp2_old_task, "301*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c:49:49*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c:0:0**" );
#line 49 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c"
    #pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads)
        { POMP2_Parallel_begin( &pomp2_region_1 );
#line 50 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c"
          {
              printf( "Hello world from process %d, thread %d of %d, %d\n",
                      rank, omp_get_thread_num(), size, omp_get_num_threads() );
          }
          { POMP2_Task_handle pomp2_old_task;
            POMP2_Implicit_barrier_enter( &pomp2_region_1, &pomp2_old_task );
#pragma omp barrier
            POMP2_Implicit_barrier_exit( &pomp2_region_1, pomp2_old_task );
          }
          POMP2_Parallel_end( &pomp2_region_1 );
        }
        POMP2_Parallel_join( &pomp2_region_1, pomp2_old_task );
    }
#line 54 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c"

    MPI_Finalize();

    return 0;
}
