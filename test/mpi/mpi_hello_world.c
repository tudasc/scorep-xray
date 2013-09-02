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
 * @file       mpi_hello_world.c
 *
 *
 */


#include <config.h>

#include <stdio.h>
#include <mpi.h>


int
main( int    argc,
      char** argv )
{
    int rank, size;

    MPI_Init( &argc, &argv );               /* starts MPI */
    MPI_Comm_rank( MPI_COMM_WORLD, &rank ); /* get current process id */
    MPI_Comm_size( MPI_COMM_WORLD, &size ); /* get number of processes */
    printf( "Hello world from process %d of %d\n", rank, size );
    MPI_Finalize();
    return 0;
}



/* *INDENT-OFF* */
/* *INDENT-ON*  */
