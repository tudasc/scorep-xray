/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
#include <scorep/SCOREP_PublicTypes.h>
#include "scorep_definition_structs.h"
#include "scorep_definition_macros.h"
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>

// the list of definitions for what we generate mappings to global
// ids in the unifier
#define SCOREP_LIST_OF_DEFS_WITH_MAPPINGS \
    DEF_WITH_MAPPING( String, string ) \
    DEF_WITH_MAPPING( Region, region ) \
    DEF_WITH_MAPPING( Group, group ) \
    DEF_WITH_MAPPING( Metric, metric ) \
    DEF_WITH_MAPPING( SamplingSet, sampling_set ) \
    DEF_WITH_MAPPING( Parameter, parameter ) \
    DEF_WITH_MAPPING( Callpath, callpath )


/* *INDENT-OFF* */
typedef struct SCOREP_DefinitionMappings SCOREP_DefinitionMappings;
struct SCOREP_DefinitionMappings
{
    #define DEF_WITH_MAPPING( Type, type ) \
    uint32_t* type ## _mappings;
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING

    /* handled special in the mpi communicator unifier code */
    uint32_t* local_mpi_communicator_mappings;
};
/* *INDENT-ON* */


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
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SystemTreeNode, system_tree_node )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( LocationGroup, location_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Location, location )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SourceFile, source_file )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Region, region )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Group, group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( LocalMPICommunicator, local_mpi_communicator )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICommunicator, mpi_communicator )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( RmaWindow, rma_window )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICartesianTopology, mpi_cartesian_topology )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MPICartesianCoords, mpi_cartesian_coords )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Metric, metric )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SamplingSet, sampling_set )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( IOFileGroup, io_file_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( IOFile, io_file )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( MarkerGroup, marker_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Marker, marker )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Parameter, parameter )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Callpath, callpath )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Property, property )

    /** The pager manager where all definition objects resides */
    SCOREP_Allocator_PageManager* page_manager;

    /** Mappings for local definition ids to global ids.
     * Used only in local definition instance.
     */
    SCOREP_DefinitionMappings* mappings;
};

/* *INDENT-ON* */

extern SCOREP_DefinitionManager scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

void
SCOREP_Definitions_Initialize();


void
SCOREP_Definitions_Finalize();


void
SCOREP_Definitions_Lock( void );

void
SCOREP_Definitions_Unlock( void );

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


void
SCOREP_CopyMetricDefinitionToUnified( SCOREP_Metric_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager );


void
SCOREP_CopySamplingSetDefinitionToUnified( SCOREP_SamplingSet_Definition* definition,
                                           SCOREP_Allocator_PageManager*  handlesPageManager );


SCOREP_SystemTreeNodeHandle
SCOREP_DefineSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                             const char*                 name,
                             const char*                 klass );

SCOREP_LocationGroupHandle
SCOREP_DefineLocationGroup( SCOREP_SystemTreeNodeHandle parent );


SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t              globalLocationId,
                       SCOREP_LocationType   type,
                       SCOREP_LocationHandle parent,
                       const char*           name );


void
SCOREP_AddClockOffset( uint64_t time,
                       int64_t  offset,
                       double   stddev );

void
SCOREP_GetFirstClockSyncPair( int64_t*  offset1,
                              uint64_t* timestamp1,
                              int64_t*  offset2,
                              uint64_t* timestamp2 );


void
SCOREP_GetLastClockSyncPair( int64_t*  offset1,
                             uint64_t* timestamp1,
                             int64_t*  offset2,
                             uint64_t* timestamp2 );


void
SCOREP_CopyLocationDefinitionToUnified( SCOREP_Location_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager );


void
SCOREP_CopyLocationGroupDefinitionToUnified( SCOREP_LocationGroup_Definition* definition,
                                             SCOREP_Allocator_PageManager*    handlesPageManager );

void
SCOREP_CopySystemTreeNodeDefinitionToUnified( SCOREP_SystemTreeNode_Definition* definition,
                                              SCOREP_Allocator_PageManager*     handlesPageManager );

void
SCOREP_CopyGroupDefinitionToUnified( SCOREP_Group_Definition*      definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager );


void
SCOREP_CopyParameterDefinitionToUnified( SCOREP_Parameter_Definition*  definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager );

void
SCOREP_CopyPropertyDefinitionToUnified( SCOREP_Property_Definition*   definition,
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


uint32_t
SCOREP_GetRegionHandleToID( SCOREP_RegionHandle handle );

#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
