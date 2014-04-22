/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/* *INDENT-OFF* */
#include <config.h>

#include <mpi.h>
#include <stdio.h>

#include <scorep/SCOREP_User.h>

int
main( int    argc,
      char** argv )
{
    int	retVal = 0; /* return value */
    int k, myrank=0, np=1;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    for(k=0;k<30;k++)
    {
        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

        printf("OA_TEST_C_MPI rank %d: Inside Online Access phase\n",myrank);

        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    MPI_Finalize();
    return retVal;
}
