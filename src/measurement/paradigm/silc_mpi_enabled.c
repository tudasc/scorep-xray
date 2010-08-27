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
 * @status alpha
 *
 *
 */

#include <config.h>
#include "silc_mpi.h"

#include <silc_utility/SILC_Debug.h>
#include <silc_thread.h>
#include <silc_definitions.h>
#include <mpi.h>
#include <assert.h>


static MPI_Comm silc_mpi_comm_world;


extern void
silc_status_initialize_mpi();


extern void
silc_status_initialize_common();


void
SILC_Status_Initialize()
{
    silc_status_initialize_common();
    silc_status_initialize_mpi();
    // it is too early to call PMPI from here.
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
    int status = PMPI_Barrier( silc_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


void
SILC_Mpi_DuplicateCommWorld()
{
    assert( SILC_Mpi_IsInitialized() );
    assert( !SILC_Mpi_IsFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &silc_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


int
SILC_Mpi_CalculateCommWorldSize()
{
    int size;
    PMPI_Comm_size( silc_mpi_comm_world, &size );
    return size;
}


int*
SILC_Mpi_GatherNumberOfLocationsPerRank()
{
    int* n_locations_per_rank = 0;
    if ( SILC_Mpi_GetRank() == 0 )
    {
        n_locations_per_rank = calloc( SILC_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( n_locations_per_rank );
    }
    int n_local_locations = SILC_Thread_GetNumberOfLocations();
    PMPI_Gather( &n_local_locations,
                 1,
                 MPI_INT,
                 n_locations_per_rank,
                 1,
                 MPI_INT,
                 0,
                 silc_mpi_comm_world );
    return n_locations_per_rank;
}


int*
SILC_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                               int  nGlobalLocations )
{
    /// @todo refactor
    int* n_definitions_per_location = 0;
    int* diplacements               = 0;
    int  rank                       = 0;
    if ( SILC_Mpi_GetRank() == 0 )
    {
        // recv buf
        n_definitions_per_location = calloc( nGlobalLocations, sizeof( int ) );
        assert( n_definitions_per_location );

        // displacements
        diplacements = calloc( SILC_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( diplacements );
        int displacement = 0;
        for ( rank = 0; rank < SILC_Mpi_GetCommWorldSize(); ++rank )
        {
            diplacements[ rank ] = displacement;
            displacement        += nLocationsPerRank[ rank ];
        }
    }

    // send buf
    int* n_local_definitions = calloc( SILC_Thread_GetNumberOfLocations(), sizeof( int ) );
    assert( n_local_definitions );
    n_local_definitions[ 0 ] = SILC_GetNumberOfDefinitions();

    PMPI_Gatherv( n_local_definitions,
                  SILC_Thread_GetNumberOfLocations(),
                  MPI_INT,
                  n_definitions_per_location,
                  nLocationsPerRank,
                  diplacements,
                  MPI_INT,
                  0,
                  silc_mpi_comm_world );

    if ( n_local_definitions )
    {
        free( n_local_definitions );
    }
    if ( diplacements )
    {
        free( diplacements );
    }
    return n_definitions_per_location;
}
