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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#define SCOREP_INTERNAL_DEFINITIONS_H


/**
 * @file       scorep_definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_PublicTypes.h>
#include "scorep_definition_structs.h"
#include "scorep_definition_macros.h"
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>


typedef struct SCOREP_DefinitionMappings SCOREP_DefinitionMappings;
struct SCOREP_DefinitionMappings
{
    //uint32_t* location_mappings;
    uint32_t* region_mappings;
    uint32_t* group_mappings;
    uint32_t* mpi_window_mappings;
    //uint32_t* gats_group_mappings; /// @todo what definition type does this correspond to?
    uint32_t* parameter_mappings;
    uint32_t* callpath_mappings;
    uint32_t* mpi_communicator_mappings;
};


/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;
struct SCOREP_DefinitionManager
{
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( String, string )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Location, location )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SourceFile, source_file )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Region, region )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Group, group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICommunicator, mpi_communicator )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPIWindow, mpi_window )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICartesianTopology, mpi_cartesian_topology )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICartesianCoords, mpi_cartesian_coords )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( CounterGroup, counter_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Counter, counter )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( IOFileGroup, io_file_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( IOFile, io_file )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MarkerGroup, marker_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Marker, marker )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Parameter, parameter )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Callpath, callpath )

    /** The pager manager where all definition objects resides */
    SCOREP_Allocator_PageManager* page_manager;

    /** Mappings for local definition ids to global ids.
     * Used only in local definition instance.
     */
    SCOREP_DefinitionMappings* mappings;
};
/* *INDENT-ON* */

/**
   Counts the number of communicators at this process which are equal to
   MPI_COMM_SELF.
 */
extern uint32_t scorep_number_of_self_comms;

/**
   Counts the number of communicators which have this rank as root and
   are not equal to MPI_COMM_SELF
 */
extern uint32_t scorep_number_of_root_comms;

void
SCOREP_Definitions_Initialize();


void
SCOREP_Definitions_Finalize();


void
SCOREP_Definitions_Lock();

void
SCOREP_Definitions_Unlock();

void
SCOREP_InitializeDefinitionManager( SCOREP_DefinitionManager**    manager,
                                    SCOREP_Allocator_PageManager* pageManager,
                                    bool                          allocHashTables );


void
SCOREP_Definitions_Write();


SCOREP_StringHandle
SCOREP_DefineString( const char* str );


void
SCOREP_CopyStringDefinitionToUnified( SCOREP_String_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager );


// for testing purposes only, use SCOREP_CopyStringDefinitionToUnified() or
// SCOREP_DefineString() instead.
SCOREP_StringHandle
scorep_string_definition_define( SCOREP_DefinitionManager* definition_manager,
                                 const char*               str );


void
SCOREP_CopySourceFileDefinitionToUnified( SCOREP_SourceFile_Definition* definition,
                                          SCOREP_Allocator_PageManager* handlesPageManager );


// for testing purposes only, use SCOREP_CopySourceFileDefinitionToUnified() or
// SCOREP_DefineSourceFile() instead.
SCOREP_SourceFileHandle
scorep_source_file_definition_define( SCOREP_DefinitionManager* definition_manager,
                                      SCOREP_StringHandle       fileNameHandle );


void
SCOREP_CopyRegionDefinitionToUnified( SCOREP_Region_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager );


// for testing purposes only, use SCOREP_CopyRegionDefinitionToUnified() or
// SCOREP_DefineRegion() instead.
SCOREP_RegionHandle
scorep_region_definition_define( SCOREP_DefinitionManager* definition_manager,
                                 SCOREP_StringHandle       regionNameHandle,
                                 SCOREP_StringHandle       descriptionNameHandle,
                                 SCOREP_StringHandle       fileNameHandle,
                                 SCOREP_LineNo             beginLine,
                                 SCOREP_LineNo             endLine,
                                 SCOREP_AdapterType        adapter,
                                 SCOREP_RegionType         regionType );


SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t              globalLocationId,
                       SCOREP_LocationHandle parent,
                       const char*           name );


void
SCOREP_CopyLocationDefinitionToUnified( SCOREP_Location_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager );


void
SCOREP_CopyGroupDefinitionToUnified( SCOREP_Group_Definition*      definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager );


void
SCOREP_CopyParameterDefinitionToUnified( SCOREP_Parameter_Definition*  definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager );


SCOREP_CallpathHandle
SCOREP_DefineCallpath( SCOREP_CallpathHandle parent,
                       SCOREP_RegionHandle   region );


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterInteger( SCOREP_CallpathHandle  parent,
                                       SCOREP_ParameterHandle param,
                                       int64_t                value );


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterString( SCOREP_CallpathHandle  parent,
                                      SCOREP_ParameterHandle param,
                                      SCOREP_StringHandle    value );


void
SCOREP_CopyCallpathDefinitionToUnified( SCOREP_Callpath_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager );


int
SCOREP_GetNumberOfDefinitions();

uint32_t
SCOREP_GetNumberOfRegionDefinitions();

uint32_t
SCOREP_GetNumberOfCounterDefinitions();

uint32_t
SCOREP_CallPathHandleToRegionID( SCOREP_CallpathHandle handle );

uint32_t
SCOREP_GetHandleToID( SCOREP_RegionHandle handle );

void
SCOREP_UpdateLocationDefinitions();

#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
