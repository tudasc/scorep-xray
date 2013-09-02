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
 *
 *
 */


#include <config.h>

#include <stdio.h>
#include <mpi.h>
#include <omp.h>


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

    #pragma omp parallel
    {
        printf( "Hello world from process %d, thread %d of %d, %d\n",
                rank, omp_get_thread_num(), size, omp_get_num_threads() );
    }

    MPI_Finalize();

    return 0;
}
