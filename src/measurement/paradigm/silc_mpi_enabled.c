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

#include <SILC_Debug.h>
#include <mpi.h>
#include <assert.h>


extern void
silc_status_initialize_mpi();


void
SILC_Status_Initialize()
{
    silc_status_initialize_mpi();
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
    //MPI_Bcast( dirName, dirNameSize, MPI_CHAR, 0, MPI_COMM_WORLD );

    return true;
}


bool
SILC_Mpi_HasMpi()
{
    return true;
}


void
SILC_Mpi_GlobalBarrier()
{
    assert( SILC_Mpi_IsInitialized() );
    assert( !SILC_Mpi_IsFinalized() );
    int status = PMPI_Barrier( MPI_COMM_WORLD );
    assert( status == MPI_SUCCESS );
}
