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


/**
 * @file       scorep_mpi_disabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include "scorep_mpi.h"

#include <scorep_thread.h>
#include <scorep_definitions.h>


extern void
scorep_status_initialize_non_mpi();


extern void
scorep_status_initialize_common();


void
SCOREP_Status_Initialize()
{
    scorep_status_initialize_common();
    scorep_status_initialize_non_mpi();
}


bool
scorep_create_experiment_dir( char* dirName,
                              int   dirNameSize,
                              void  ( * createDir )( const char* ) )
{
    createDir( dirName );
    return true;
}


bool
SCOREP_Mpi_HasMpi()
{
    return false;
}


void
SCOREP_Mpi_GlobalBarrier()
{
    // nothing to to here
}


void
SCOREP_Mpi_DuplicateCommWorld()
{
    // nothing to do here
}


int
SCOREP_Mpi_CalculateCommWorldSize()
{
    return 1;
}


int*
SCOREP_Mpi_GatherNumberOfLocationsPerRank()
{
    int* n_locations_per_rank = malloc( SCOREP_Mpi_GetCommWorldSize() * sizeof( int ) );
    assert( n_locations_per_rank );
    *n_locations_per_rank = SCOREP_Thread_GetNumberOfLocations();
    return n_locations_per_rank;
}


int*
SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                                 int  nGlobalLocations )
{
    int* n_definitions_per_location = calloc( nGlobalLocations, sizeof( int ) );
    assert( n_definitions_per_location );
    n_definitions_per_location[ 0 ] = SCOREP_GetNumberOfDefinitions();
    return n_definitions_per_location;
}
