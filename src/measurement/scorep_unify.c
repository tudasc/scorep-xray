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
 * @file       scorep_unify.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "scorep_unify.h"

#include <SCOREP_Config.h>
#include "scorep_environment.h"
#include "scorep_mpi.h"
#include "scorep_definitions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <scorep_utility/SCOREP_Debug.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;
extern SCOREP_DefinitionManager* scorep_remote_definition_manager;


//typedef struct scorep_unify_string_definition scorep_unify_string_definition;
//struct scorep_unify_string_definition
//{
//    SCOREP_String_Definition*       definition;
//    scorep_unify_string_definition* next;
//};


/* *INDENT-OFF* */
/* *INDENT-ON* */


void
SCOREP_Unify( void )
{
    SCOREP_UpdateLocationDefinitions();
    if ( SCOREP_Mpi_HasMpi() )
    {
        SCOREP_Mpi_Unify();
    }
    else
    {
        SCOREP_Unify_Locally();
    }
}


void
SCOREP_CopyDefinitionsToUnified( SCOREP_DefinitionManager* sourceDefinitionManager )
{
    assert( sourceDefinitionManager );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, String, string );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, SystemTreeNode, system_tree_node );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, LocationGroup, location_group );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Location, location );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, SourceFile, source_file );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Region, region );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Group, group );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, MPIWindow, mpi_window );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, MPICartesianTopology, mpi_cartesian_topology );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, MPICartesianCoords, mpi_cartesian_coords );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, CounterGroup, counter_group );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Counter, counter );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, IOFileGroup, io_file_group );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, IOFile, io_file );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, MarkerGroup, marker_group );
//    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Marker, marker );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Parameter, parameter );
    SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( sourceDefinitionManager, Callpath, callpath );
}


void
SCOREP_CreateDefinitionMappings( SCOREP_DefinitionManager* definitionManager )
{
    assert( definitionManager );
    assert( !definitionManager->mappings );

    definitionManager->mappings = malloc( sizeof( *definitionManager->mappings ) );
    assert( definitionManager->mappings );

    //SCOREP_ALLOC_MAPPINGS_ARRAY( location, definitionManager );
    SCOREP_ALLOC_MAPPINGS_ARRAY( region, definitionManager );
    SCOREP_ALLOC_MAPPINGS_ARRAY( group, definitionManager );
    //SCOREP_ALLOC_MAPPINGS_ARRAY( mpi_window, definitionManager );
    //SCOREP_ALLOC_MAPPINGS_ARRAY( gats_group, definitionManager );
    SCOREP_ALLOC_MAPPINGS_ARRAY( parameter, definitionManager );
    SCOREP_ALLOC_MAPPINGS_ARRAY( callpath,  definitionManager );

    // will be done separately in the MPI unify code
    //SCOREP_ALLOC_MAPPINGS_ARRAY( local_mpi_communicator,  definitionManager );
}


void
SCOREP_AssignDefinitionMappingsFromUnified( SCOREP_DefinitionManager* definitionManager )
{
    assert( definitionManager );
    assert( definitionManager->mappings );

    //SCOREP_ASSIGN_MAPPINGS( definitionManager, Location, location );
    SCOREP_ASSIGN_MAPPINGS( definitionManager, Region, region );
    SCOREP_ASSIGN_MAPPINGS( definitionManager, Group, group );
    //SCOREP_ASSIGN_MAPPINGS( definitionManager, MPIWindow, mpi_window );
    //SCOREP_ASSIGN_MAPPINGS( definitionManager, , gats_group );
    SCOREP_ASSIGN_MAPPINGS( definitionManager, Parameter, parameter );
    SCOREP_ASSIGN_MAPPINGS( definitionManager, Callpath, callpath );
}


void
SCOREP_DestroyDefinitionMappings( SCOREP_DefinitionManager* definitionManager )
{
    assert( definitionManager );
    assert( definitionManager->mappings );

    //SCOREP_FREE_MAPPINGS_ARRAY( location, definitionManager );
    SCOREP_FREE_MAPPINGS_ARRAY( region, definitionManager );
    SCOREP_FREE_MAPPINGS_ARRAY( group, definitionManager );
    //SCOREP_FREE_MAPPINGS_ARRAY( mpi_window, definitionManager );
    //SCOREP_FREE_MAPPINGS_ARRAY( gats_group, definitionManager );
    SCOREP_FREE_MAPPINGS_ARRAY( parameter, definitionManager );
    SCOREP_FREE_MAPPINGS_ARRAY( callpath,  definitionManager );

    free( definitionManager->mappings );
    definitionManager->mappings = NULL;
}


void
SCOREP_Unify_Locally( void )
{
    assert( scorep_unified_definition_manager == 0 );

    bool alloc_hash_tables = true;
    SCOREP_InitializeDefinitionManager( &scorep_unified_definition_manager,
                                        SCOREP_Memory_GetLocalDefinitionPageManager(),
                                        alloc_hash_tables );
    // define empty string, used in scorep_write_number_of_definitions_per_location_to_otf2()
    // for the location name.
    scorep_string_definition_define( scorep_unified_definition_manager, "" );
    SCOREP_CopyDefinitionsToUnified( &scorep_local_definition_manager );
    // The unified definitions might differ from the local ones if there were
    // duplicates in the local ones. By creating mappings we are on the save side.
    SCOREP_CreateDefinitionMappings( &scorep_local_definition_manager );
    SCOREP_AssignDefinitionMappingsFromUnified( &scorep_local_definition_manager );
}
