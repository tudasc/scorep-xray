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

/* *INDENT-OFF* */
#include <config.h>

#include <mpi.h>
#include <stdio.h>

#include "SCOREP_User.h"

double test1(double *a, int rank, int size);
double test2(double *a, int rank, int size);
double test3(double *a, int rank, int size);
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


    for(k=0;k<30;k++)
    {
        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

        printf("add is inside the phase, iteration %d\n", k);
        s=test1(a,myrank,10000);
        s=test2(a,myrank,10000);
        s=test3(a,myrank,10000);

        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    if (myrank==0)
        printf("hello %g\n", a[100]);

    MPI_Finalize();

    return retVal;
}

double test1(double *a, int rank, int size)
{
	double s=0;
	int i;
	for(i=0;i<size;i++)
			a[i] = 500*i;

	for(i=0;i<size;i++)
			s += a[i];

	MPI_Barrier(MPI_COMM_WORLD);
	return s;
}

double test2(double *a, int rank, int size)
{
	double s=0;
	int i;
	if(rank==0)
	{
		for(i=0;i<size;i++)
				a[i] = 505*i;

		for(i=0;i<size;i++)
				s += a[i];
	}
	else
		s=a[size-1];
	MPI_Barrier(MPI_COMM_WORLD);
	return s;
}

double test3(double *a, int rank, int size)
{
	double s=test1(a,rank,size);
	return s;
}
