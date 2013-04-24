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
 * @file       src/measurement/definitions/SCOREP_Definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>

/* forward decl */
struct SCOREP_DefinitionManager;
typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;

#include "scorep_definitions_private.h"

#include "scorep_definitions_string.h"
#include "scorep_definitions_source_file.h"
#include "scorep_definitions_location_group.h"
#include "scorep_definitions_location.h"
#include "scorep_definitions_system_tree_node.h"
#include "scorep_definitions_region.h"
#include "scorep_definitions_communicator.h"
#include "scorep_definitions_group.h"
#include "scorep_definitions_metric.h"
#include "scorep_definitions_sampling_set.h"
#include "scorep_definitions_parameter.h"
#include "scorep_definitions_callpath.h"
#include "scorep_definitions_clock_offset.h"
#include "scorep_definitions_property.h"
#include "scorep_definitions_rma_window.h"
#include "scorep_definitions_topology.h"
#include "scorep_definitions_io_file_group.h"
#include "scorep_definitions_io_file.h"
#include "scorep_definitions_marker_group.h"
#include "scorep_definitions_marker.h"

/* super object for all definitions */
SCOREP_DEFINE_DEFINITION_TYPE( Any )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Any );
};


static inline uint32_t
scorep_handle_to_id( SCOREP_AnyHandle handle )
{
    if ( handle == SCOREP_MOVABLE_NULL )
    {
        return UINT32_MAX;
    }

    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Any );
}


// the list of definitions for what we generate mappings to global
// ids in the unifier
#define SCOREP_LIST_OF_DEFS_WITH_MAPPINGS \
    DEF_WITH_MAPPING( String, string ) \
    DEF_WITH_MAPPING( Region, region ) \
    DEF_WITH_MAPPING( Group, group ) \
    DEF_WITH_MAPPING( Communicator, communicator ) \
    DEF_WITH_MAPPING( RmaWindow, rma_window ) \
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

    /* handled special in the unifier code */
    uint32_t* interim_communicator_mappings;
    uint32_t* interim_rma_window_mappings;
};
/* *INDENT-ON* */


/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
struct SCOREP_DefinitionManager
{
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( String, string )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SystemTreeNode, system_tree_node )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( LocationGroup, location_group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Location, location )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( SourceFile, source_file )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Region, region )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Group, group )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( InterimCommunicator, interim_communicator )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Communicator, communicator )
    SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( InterimRmaWindow, interim_rma_window )
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
SCOREP_Definitions_Initialize( void );


void
SCOREP_Definitions_Finalize( void );


void
SCOREP_Definitions_Lock( void );

void
SCOREP_Definitions_Unlock( void );

void
SCOREP_InitializeDefinitionManager( SCOREP_DefinitionManager**    manager,
                                    SCOREP_Allocator_PageManager* pageManager,
                                    bool                          allocHashTables );


void
SCOREP_Definitions_Write( void );


#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
