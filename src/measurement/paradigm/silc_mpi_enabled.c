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
#include <SILC_Debug.h>

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


bool
silc_create_experiment_dir( char* dirName,
                            int   dirNameSize,
                            void  ( * createDir )( const char* ) )
{
    if ( !SILC_Mpi_IsInitialized() )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SILC_Mpi_GetRank() == 0 )
    {
        createDir( dirName );
    }
    MPI_Bcast( dirName, dirNameSize, MPI_CHAR, 0, MPI_COMM_WORLD );

    return true;
}


bool
SILC_Mpi_HasMpi()
{
    return true;
}


bool
SILC_Mpi_IsInitialized()
{
    int is_initialized;
    MPI_Initialized( &is_initialized );
    return is_initialized;
}
