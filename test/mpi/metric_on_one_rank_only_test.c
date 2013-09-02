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
 * @file       metric_on_one_rank_only_test.c
 *
 *
 */

#include <config.h>
#include <stdio.h>
#include <mpi.h>
#include <scorep/SCOREP_User.h>

int
main( int    argc,
      char** argv )
{
    int rank, size;

    MPI_Init( &argc, &argv );               /* starts MPI */
    MPI_Comm_rank( MPI_COMM_WORLD, &rank ); /* get current process id */
    MPI_Comm_size( MPI_COMM_WORLD, &size ); /* get number of processes */


    if ( rank == 0 )
    {
        printf( "Run metric_on_one_rank_only_test\n" );
        SCOREP_USER_METRIC_LOCAL( localMetric );
        SCOREP_USER_METRIC_INIT( localMetric, "localMetric2", "s",
                                 SCOREP_USER_METRIC_TYPE_DOUBLE,
                                 SCOREP_USER_METRIC_CONTEXT_CALLPATH );
        SCOREP_USER_METRIC_DOUBLE( localMetric, 5.5 );
    }

    if ( rank == 1 )
    {
        SCOREP_USER_METRIC_LOCAL( localMetric );
        SCOREP_USER_METRIC_INIT( localMetric, "localMetric1", "s",
                                 SCOREP_USER_METRIC_TYPE_UINT64,
                                 SCOREP_USER_METRIC_CONTEXT_CALLPATH );
        SCOREP_USER_METRIC_UINT64( localMetric, 3 );
    }

    MPI_Finalize();
    return 0;
}
