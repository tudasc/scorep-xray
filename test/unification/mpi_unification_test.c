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
 * @file       mpi_unification_test.c
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>

#include <SCOREP_Types.h>
#include <SCOREP_Memory.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>

#include <scorep_definitions.h>

void
SCOREP_Mpi_Unify( void );
void
SCOREP_Definitions_Initialize( void );
void
SCOREP_Mpi_SetRankTo( int rank );
int
SCOREP_Mpi_GetRank( void );
void
SCOREP_Mpi_GlobalBarrier( void );

extern SCOREP_DefinitionManager* scorep_unified_definition_manager;
extern SCOREP_DefinitionManager  scorep_local_definition_manager;

int
main( int argc, char* argv[] )
{
    SCOREP_Memory_Initialize( 8 << 20, 1 << 12 );
    SCOREP_Definitions_Initialize();

    PMPI_Init( &argc, &argv );

    SCOREP_OnPMPI_Init();

    int size;
    PMPI_Comm_size( MPI_COMM_WORLD, &size );

    int rank;
    PMPI_Comm_rank( MPI_COMM_WORLD, &rank );

    SCOREP_Mpi_SetRankTo( rank );

    char rank_buffer[ 32 ];
    sprintf( rank_buffer, "Rank %d", rank );

    SCOREP_DefineRegion( rank_buffer,
                         0, //SCOREP_DefineSourceFile( __FILE__ ),
                         51,
                         80,
                         SCOREP_ADAPTER_USER,
                         SCOREP_REGION_FUNCTION );

    SCOREP_Mpi_Unify();

    if ( rank == 0 )
    {
        FILE* result = fopen( "mpi_unification_test.result", "w" );
        assert( result );
        SCOREP_DEFINITION_FOREACH_DO( scorep_unified_definition_manager,
                                      Region, region )
        {
            fprintf( result, "u: %u (%s)\n",
                     definition->sequence_number,
                     SCOREP_HANDLE_DEREF( definition->name_handle, String, scorep_unified_definition_manager->page_manager )->string_data );
        }
        SCOREP_DEFINITION_FOREACH_WHILE();
        fclose( result );
    }
    SCOREP_Mpi_GlobalBarrier();


    for ( int the_rank = 0; the_rank < size; the_rank++ )
    {
        if ( the_rank == rank )
        {
            FILE* result = fopen( "mpi_unification_test.result", "a" );
            assert( result );

            SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                          Region, region )
            {
                fprintf( result, "%d: %u -> %u\n",
                         SCOREP_Mpi_GetRank(),
                         definition->sequence_number,
                         scorep_local_definition_manager.mappings->region_mappings[ definition->sequence_number ] );
            }
            SCOREP_DEFINITION_FOREACH_WHILE();
            fclose( result );
        }

        SCOREP_Mpi_GlobalBarrier();
    }

    PMPI_Finalize();

    SCOREP_OnPMPI_Finalize();

    return 0;
}
