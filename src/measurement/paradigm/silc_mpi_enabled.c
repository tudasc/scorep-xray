/*
 * This file is part of the SILC project (http://www.silc.de)
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


/**
 * @file       silc_mpi_enabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_mpi.h"

#include <mpi.h>


uint64_t
SILC_Mpi_GetRank()
{
    int rank = 0;
    int is_initialized;
    MPI_Initialized( &is_initialized );
    if ( is_initialized )
    {
        MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    }
    return rank;
}
