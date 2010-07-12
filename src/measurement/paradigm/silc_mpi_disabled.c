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
 * @file       silc_mpi_disabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_mpi.h"

#include <silc_thread.h>
#include <silc_definitions.h>


extern void
silc_status_initialize_non_mpi();


extern void
silc_status_initialize_common();


void
SILC_Status_Initialize()
{
    silc_status_initialize_common();
    silc_status_initialize_non_mpi();
}


bool
silc_create_experiment_dir( char* dirName,
                            int   dirNameSize,
                            void  ( * createDir )( const char* ) )
{
    createDir( dirName );
    return true;
}


bool
SILC_Mpi_HasMpi()
{
    return false;
}


void
SILC_Mpi_GlobalBarrier()
{
    // nothing to to here
}


void
SILC_Mpi_DuplicateCommWorld()
{
    // nothing to do here
}


int
SILC_Mpi_CalculateCommWorldSize()
{
    return 1;
}


int*
SILC_Mpi_GatherNumberOfLocationsPerRank()
{
    int* n_locations_per_rank = malloc( SILC_Mpi_GetCommWorldSize() * sizeof( int ) );
    assert( n_locations_per_rank );
    *n_locations_per_rank = SILC_Thread_GetNumberOfLocations();
    return n_locations_per_rank;
}


int*
SILC_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                               int  nGlobalLocations )
{
    int* n_definitions_per_location = calloc( nGlobalLocations, sizeof( int ) );
    assert( n_definitions_per_location );
    n_definitions_per_location[ 0 ] = SILC_GetNumberOfDefinitions();
    return n_definitions_per_location;
}
