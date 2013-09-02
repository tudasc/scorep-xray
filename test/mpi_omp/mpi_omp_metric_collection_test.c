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
 * @file       mpi_omp_metric_collection_test.c
 *
 *
 */

#include <config.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <scorep/SCOREP_User.h>

SCOREP_USER_METRIC_GLOBAL( global_metric );
SCOREP_USER_METRIC_GLOBAL( process_metric );
SCOREP_USER_METRIC_GLOBAL( thread_metric );


void
func( int num )
{
    SCOREP_USER_PARAMETER_UINT64( "num", num );
    int rank;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    if ( rank == num )
    {
        SCOREP_USER_METRIC_UINT64( process_metric, 1 );
    }

  #pragma omp parallel
    {
        if ( omp_get_thread_num() == num )
        {
            SCOREP_USER_METRIC_DOUBLE( thread_metric, 1.0 );
        }
    }

    SCOREP_USER_METRIC_UINT64( global_metric, 1 );
}

int
main( int argc, char** argv )
{
    MPI_Init( &argc, &argv );

    SCOREP_USER_METRIC_INIT( global_metric, "global metric", "",
                             SCOREP_USER_METRIC_TYPE_UINT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );

    SCOREP_USER_METRIC_INIT( process_metric, "process metric", "",
                             SCOREP_USER_METRIC_TYPE_UINT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );

    SCOREP_USER_METRIC_INIT( thread_metric, "thread metric", "",
                             SCOREP_USER_METRIC_TYPE_DOUBLE,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );

    func( 0 );
    func( 1 );
    func( 2 );
    func( 3 );

    MPI_Finalize();
}
