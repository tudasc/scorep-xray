/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/* *INDENT-OFF* */
#include <config.h>

#include <mpi.h>
#include <stdio.h>

#include "SCOREP_User.h"

int
main( int    argc,
      char** argv )
{
    int	retVal = 0; /* return value */
    double a[10000], s;
    int i,k, myrank=0, np=1;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (myrank==0)
        printf("add example started with %d processes\n", np);

    SCOREP_USER_REGION_DEFINE( mainRegion );
    SCOREP_USER_OA_PHASE_BEGIN( mainRegion );
    SCOREP_USER_REGION_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON );

    for(k=0;k<30;k++)
    {

        for(i=0;i<10000;i++)
                a[i] = 500*i;

        for(i=0;i<10000;i++)
                s += a[i];

        MPI_Barrier(MPI_COMM_WORLD);
    }

    SCOREP_USER_REGION_END( mainRegion );
    SCOREP_USER_OA_PHASE_END( mainRegion );

    if (myrank==0)
        printf("hello %g\n", a[100]);

        MPI_Finalize();

    return retVal;
}
